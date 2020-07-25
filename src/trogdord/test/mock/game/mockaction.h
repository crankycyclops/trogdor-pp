#ifndef MOCK_ACTION_H
#define MOCK_ACTION_H


#include <functional>
#include <trogdor/actions/action.h>


// Simple mock action used to execute arbitrary callbacks in response to player
// input
class MockAction: public trogdor::Action {

	private:

		// The actual code to be run when execute() is called.
		std::function<void()> callback;

	public:

		// Constructor
		inline MockAction(std::function<void()> c = {}) {

			callback = c;
		}

		virtual bool checkSyntax(const trogdor::Command &command);

		virtual void execute(
			trogdor::entity::Player *player,
			const trogdor::Command &command,
			trogdor::Game *game
		);
};


#endif
