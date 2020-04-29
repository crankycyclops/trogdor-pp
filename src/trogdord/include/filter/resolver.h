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

			bool started = false;
			std::set<T> result;

			if (1 == group.size()) {
				return resolveFilter(*group.begin());
			}

			// TODO: this solution seems very suboptimal. I'll have to
			// investigate what I'm doing further.
			for (auto const &filter: group) {

				if (!started) {
					result = std::move(resolveFilter(filter));
					started = true;
				}

				else {

					// Simple optimization: if we know the current result
					// is empty, we don't have to compare the other sets.
					if (!result.size()) {
						return {};
					}

					std::set<T> filterSet = resolveFilter(filter);
					std::set<T> intersection;

					std::set_intersection(
						result.begin(),
						result.end(),
						filterSet.begin(),
						filterSet.end(),
						std::inserter(intersection, intersection.begin())
					);

					result = std::move(intersection);
				}
			}

			return result;
		}

	public:

		// Deregisters a previously supported filter
		void removeRule(std::string filterType) {

			if (rules.end() != rules.find(filterType)) {
				rules.erase(filterType);
			}
		}

		// Registers a new supported filter
		void addRule(std::string filterType, std::function<std::set<T>(Filter)> callback) {

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
