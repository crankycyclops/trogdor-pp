#include "mockentity.h"

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

namespace trogdor::entity {


	MockEntity::MockEntity(
		Game *g,
		std::string n
	): Entity(g, n, std::make_unique<NullOut>(), std::make_unique<NullErr>()) {

		setValidationRules();
	}

	/************************************************************************/

	MockEntity::MockEntity(
		Game *g,
		std::string n,
		std::unique_ptr<Trogout> o,
		std::unique_ptr<Trogerr> e
	): Entity(g, n, std::move(o), std::move(e)) {

		setValidationRules();
	}

	/************************************************************************/

	MockEntity::~MockEntity() {}

	/************************************************************************/

	void MockEntity::setValidationRules() {

		// The property named "forceString" must be a string; it can't be
		// any other type.
		setPropertyValidator("forceString", [](PropertyValue v) -> int {
			return 4 == v.index() ? PROPERTY_VALID : PROPERTY_NOT_STRING;
		});
	}
}
