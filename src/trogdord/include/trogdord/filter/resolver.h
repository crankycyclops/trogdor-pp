#ifndef FILTER_RESOLVER_H
#define FILTER_RESOLVER_H

#include <set>
#include <string>
#include <unordered_map>
#include <functional>
#include <algorithm>

#include "filter.h"
#include "../exception/filterexception.h"


template <typename T>
class FilterResolver {

	private:

		// Defines the filters that are accepted and provides callbacks which
		// take as input one of those filters with a value and returns the
		// corresponding set.
		std::unordered_map<std::string, std::function<std::set<T>(Filter)>> rules;

		// Reduces a single filter to a set
		std::set<T> resolveFilter(Filter filter) {

			if (rules.end() == rules.find(filter.getType())) {
				throw FilterException(
					"Unsupported filter '" +
					filter.getType() + "'"
				);
			}

			try {
				return rules[filter.getType()](filter);
			}

			catch (const std::bad_variant_access &e) {
				throw FilterException(
					std::string("Invalid value for filter '") +
					filter.getType() + "'"
				);
			}
		}

		// Reduces a filter group to a set
		std::set<T> resolveGroup(Filter::Group group) {

			// If we only have one filter in the group, we can just return
			// that filter's result.
			if (1 == group.size()) {
				return resolveFilter(*group.begin());
			}

			// This is where the intersection for the whole group will be
			// stored.
			std::set<T> intersection;

			// This is where we store each filter's result until we're ready
			// to calculate their intersection.
			std::vector<std::set<T>> intermediate;

			// The basic idea here is this: we're going to resolve filters
			// sequentially, and if any of them return an empty result, we
			// know the whole filter group is going to be empty and we can
			// return an empty set without having to calculate their
			// intersection. If, on the other hand, all of the resulting sets
			// contain at least one item, we know we'll have to complete the
			// operation.
			for (auto const &filter: group) {

				intermediate.push_back(resolveFilter(filter));

				// Yay! Since a filter group is basically an AND and since
				// one of our filters returned an empty result, we know we
				// can return an empty set for the whole group.
				if (!intermediate.back().size()) {
					return {};
				}
			}

			// We got a non-zero result for each filter, so we'll have to
			// take some time to calculate their intersection.
			auto i = intermediate.begin();
			intersection = *i;

			for (i++; i != intermediate.end(); i++) {

				std::set<T> newIntersection;

				std::set_intersection(
					intersection.begin(),
					intersection.end(),
					(*i).begin(),
					(*i).end(),
					std::inserter(newIntersection, newIntersection.begin())
				);

				intersection = std::move(newIntersection);
			}

			return intersection;
		}

	public:

		// Deregisters a previously supported filter
		void removeRule(std::string filterType) {

			if (rules.end() != rules.find(filterType)) {
				rules.erase(filterType);
			}
		}

		// Registers a new supported filter
		void addRule(const std::string &filterType, const std::function<std::set<T>(Filter)> &callback) {

			// If a rule's been defined twice, it's definitely a bug. If
			// instead, the developer is trying to do something clever, they
			// can call removeRule first.
			if (rules.end() != rules.find(filterType)) {
				throw new FilterException(
					std::string("Filter rule '") +
					filterType +
					"' was already added."
				);
			}

			rules[filterType] = callback;
		}

		// Reduces an entire filter union to a set
		std::set<T> resolve(Filter::Union filters) {

			bool started = false;
			std::set<T> result;

			if (1 == filters.size()) {
				return resolveGroup(*filters.begin());
			}

			// TODO: this solution seems very suboptimal. I'll have to
			// investigate what I'm doing further.
			for (auto const &filterGroup: filters) {

				if (!started) {
					result = std::move(resolveGroup(filterGroup));
					started = true;
				}

				else {

					std::set<T> filterGroupSet = resolveGroup(filterGroup);
					std::set<T> unionSet;

					// Nothing to merge for this particular group
					if (!filterGroupSet.size()) {
						continue;
					}

					std::set_union(
						result.begin(),
						result.end(),
						filterGroupSet.begin(),
						filterGroupSet.end(),
						std::inserter(unionSet, unionSet.begin())
					);

					result = std::move(unionSet);
				}
			}

			return result;
		}
};


#endif
