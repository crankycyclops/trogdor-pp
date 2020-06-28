#include "mockaction.h"


bool MockAction::checkSyntax(const std::shared_ptr<trogdor::Command> &command) {

	return true;
}

void MockAction::execute(
	trogdor::entity::Player *player,
	const std::shared_ptr<trogdor::Command> &command,
	trogdor::Game *game
) {

	return;
}
