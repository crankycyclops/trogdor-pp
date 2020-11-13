#ifndef MOCK_ENTITY_H
#define MOCK_ENTITY_H


#include <trogdor/entities/entity.h>

namespace trogdor::entity {


	class MockEntity: public Entity {

		private:

			// Sets test property validation rules
			void setValidationRules();

		public:

			// Property validator throws this if the value wasn't a string
			static constexpr int PROPERTY_NOT_STRING = 1;

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
	};
}


#endif // MOCK_ENTITY_H
