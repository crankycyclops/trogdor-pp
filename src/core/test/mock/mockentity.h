#ifndef MOCK_ENTITY_H
#define MOCK_ENTITY_H


#include <trogdor/entities/entity.h>

namespace trogdor::entity {


	class MockEntity: public Entity {

		public:

			// Constructor with nullout and nullerr streams
			MockEntity(
				Game *g,
				std::string n
			);

			// Constructor with custom output and error streams
			MockEntity(
				Game *g,
				std::string n,
				std::unique_ptr<Trogout> o,
				std::unique_ptr<Trogerr> e
			);

			// Destructor
			virtual ~MockEntity();

			// Wrapper providing a public interface to
			// Entity::setPropertyValidator for testing
			void setPropertyValidator(
				std::string key,
				std::function<int(PropertyValue)> validator
			);
	};
}


#endif // MOCK_ENTITY_H
