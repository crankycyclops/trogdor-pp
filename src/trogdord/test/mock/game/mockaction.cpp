#include "mockaction.h"


bool MockAction::checkSyntax(const trogdor::Command &command) {

	// The syntax really doesn't matter, so whateves
	return true;
}

void MockAction::execute(
	trogdor::entity::Player *player,
	const trogdor::Command &command,
	trogdor::Game *game
) {

	callback();
}
