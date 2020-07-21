#include "mockaction.h"


bool MockAction::checkSyntax(const std::shared_ptr<trogdor::Command> &command) {

	// The syntax really doesn't matter, so whateves
	return true;
}

void MockAction::execute(
	trogdor::entity::Player *player,
	const std::shared_ptr<trogdor::Command> &command,
	trogdor::Game *game
) {

	callback();
}
