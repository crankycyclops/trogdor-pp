#include <trogdor/lua/luastate.h>
#include <trogdor/entities/entity.h>


int luaReturnTrue(lua_State *L) {

	lua_pushboolean(L, true);
	return 1;
}

/*****************************************************************************/

// Tests luaL_checkudata_ex() by taking as input an entity and a type and
// returning true if an argument of that type could be read from the stack and
// false if not.
int luaTestCheckudataex(lua_State *L) {

	bool success = false;
	std::string errorMsg;
	const char **types = nullptr;

	int n = lua_gettop(L);

	if (n != 2) {
		return luaL_error(L, "function requires two arguments: entity and type (string)");
	}

	std::string type = lua_tostring(L, -1);

	switch (trogdor::entity::Entity::strToType(type)) {

		case trogdor::entity::ENTITY_ENTITY:

			types = trogdor::entity::LuaEntity::types;
			break;

		case trogdor::entity::ENTITY_RESOURCE:

			types = trogdor::entity::LuaResource::types;
			break;

		case trogdor::entity::ENTITY_TANGIBLE:

			types = trogdor::entity::LuaTangible::types;
			break;

		case trogdor::entity::ENTITY_PLACE:

			types = trogdor::entity::LuaPlace::types;
			break;

		case trogdor::entity::ENTITY_ROOM:

			types = trogdor::entity::LuaRoom::types;
			break;

		case trogdor::entity::ENTITY_THING:

			types = trogdor::entity::LuaThing::types;
			break;

		case trogdor::entity::ENTITY_OBJECT:

			types = trogdor::entity::LuaObject::types;
			break;

		case trogdor::entity::ENTITY_BEING:

			types = trogdor::entity::LuaBeing::types;
			break;

		case trogdor::entity::ENTITY_PLAYER:

			types = trogdor::entity::LuaPlayer::types;
			break;

		case trogdor::entity::ENTITY_CREATURE:

			types = trogdor::entity::LuaCreature::types;
			break;

		// Undefined type. Count it as a failure.
		default:
			lua_pushboolean(L, false);
			return 1;
	}

	if (trogdor::LuaState::luaL_checkudata_ex(L, -2, types)) {
		success = true;
	}

	lua_pushboolean(L, success);
	return 1;
}