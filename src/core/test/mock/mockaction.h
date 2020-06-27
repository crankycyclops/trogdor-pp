#include <trogdor/action.h>

class MockAction: public trogdor::Action {

	virtual bool checkSyntax(const std::shared_ptr<trogdor::Command> &command);

	virtual void execute(
		trogdor::entity::Player *player,
		const std::shared_ptr<trogdor::Command> &command,
		trogdor::Game *game
	);
};
