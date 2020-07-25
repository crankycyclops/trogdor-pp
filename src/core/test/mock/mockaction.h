#include <trogdor/actions/action.h>

class MockAction: public trogdor::Action {

	virtual bool checkSyntax(const trogdor::Command &command);

	virtual void execute(
		trogdor::entity::Player *player,
		const trogdor::Command &command,
		trogdor::Game *game
	);
};
