#include "mockaction.h"


bool MockAction::checkSyntax(const trogdor::Command &command) {

	return true;
}

void MockAction::execute(
	trogdor::entity::Player *player,
	const trogdor::Command &command,
	trogdor::Game *game
) {

	return;
}
