#include "mockentity.h"

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

namespace trogdor::entity {


	MockEntity::MockEntity(
		Game *g,
		std::string n
	): Entity(g, n, std::make_unique<NullOut>(), std::make_unique<NullErr>()) {}

	/************************************************************************/

	MockEntity::MockEntity(
		Game *g,
		std::string n,
		std::unique_ptr<Trogout> o,
		std::unique_ptr<Trogerr> e
	): Entity(g, n, std::move(o), std::move(e)) {}

	/************************************************************************/

	MockEntity::~MockEntity() {}

	/************************************************************************/

	void MockEntity::setPropertyValidator(
		std::string key,
		std::function<int(PropertyValue)> validator
	) {

		Entity::setPropertyValidator(key, validator);
	}
}
