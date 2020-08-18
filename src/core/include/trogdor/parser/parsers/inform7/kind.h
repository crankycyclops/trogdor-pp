#ifndef KIND_H
#define KIND_H

#include <list>
#include <string>
#include <memory>
#include <unordered_set>
#include <functional>

#include <trogdor/entities/entity.h>

namespace trogdor {


	// Kinds are Inform 7's folksy term for classes. We use a tree to represent
	// this hierarchy, with a single node at the top
	class Kind {

		private:

			// The kind's name
			std::string name;

			// The kind's internal Entity type
			entity::EntityType internalType;

			// These are properties that are always true and must always be
			// set for the given kind
			std::unordered_set<std::string> properties;

			// Each kind can have one or more children that inherit from it
			std::list<std::unique_ptr<Kind>> children;

			// Link back to the parent kind
			Kind *parent = nullptr;

			/* Internally, the engine has its own prototypical Entities
			   called "classes" (not to be confused with an actual class.)
			   These "classes" are defined in the AST used by the
			   instantiator to build a game. When we define an entity of a
			   certain kind in Inform 7, we first pull into the AST nodes to
			   define an internal "class" with the appropriate properties.
			   The ASTCallback methods for the entire inheritance hierarchy
			   of a referenced kind will be called in reverse order, starting
			   from the root type down, and will be responsible for inserting
			   the necessary AST nodes.
			*/
			std::function<void(size_t)> ASTCallback;

		public:

			Kind() = delete;
			Kind(const Kind &) = delete;

			// Constructor
			inline Kind(
				std::string name,
				entity::EntityType type,
				std::function<void(size_t)> ASTCallback = {}
			): name(name), internalType(type), ASTCallback(ASTCallback) {}

			/*
				Return's the kind's parent kind.

				Input:
					(none)

				Output:
					A pointer back to the parent (Kind *)
			*/
			inline Kind *getParent() const {return parent;}

			/*
				Return's the kind's name.

				Input:
					(none)

				Output:
					The kind's name (std::string)
			*/
			inline std::string getName() const {return name;}

			/*
				Append a new child kind.

				Input:
					Name of the new kind (std::string)
					Internal Entity type the kind maps to (entity::EntityType)
					Callback to insert the appropriate AST nodes (std::function<void(size_t)>)

				Output:
					A pointer back to the child (Kind *)
			*/
			inline Kind * appendChild(
				std::string name,
				entity::EntityType type,
				std::function<void(size_t)> ASTCallback
			) {

				std::unique_ptr<Kind> child = std::make_unique<Kind>(name, type, ASTCallback);
				Kind *childPtr = child.get();

				children.push_back(std::move(child));
				childPtr->parent = this;

				return childPtr;
			}

			/*
				Return the properties that must always be set for the kind.

				Input:
					(none)

				Output:
					All properties that must always be true for this kind
			*/
			inline const std::unordered_set<std::string> &getProperties() const {

				return properties;
			}

			/*
				Sets a property that must always be set for the given kind.

				Input:
					(none)

				Output:
					All properties that must always be true for this kind
			*/
			inline void insertProperty(std::string property) {

				if (properties.end() == properties.find(property)) {
					properties.insert(property);
				}
			}

			/*
				Returns true if the passed in kind is in another kind's
				inheritance hierarchy.

				Input:
					Kind to compare (Kind *)

				Output:
					True if the passed in kind is in the calling kind's
					inheritance hierarchy and false if not
			*/
			inline bool isKindRelated(Kind *kPtr) {

				Kind *curKind = this;

				while (curKind) {
					if (curKind == kPtr) {
						return true;
					} else {
						curKind = curKind->parent;
					}
				}

				return false;
			}
	};
}


#endif
