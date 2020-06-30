#ifndef MOCK_TRIGGER_H
#define MOCK_TRIGGER_H


#include <functional>
#include <trogdor/event/eventtrigger.h>


class MockTrigger: public trogdor::event::EventTrigger {

	private:

		// Set during construction, these values determine the trigger's
		// return value
		bool allowAction;
		bool continueExecution;

		// This gets called by execute().
		std::function<void()> executeCallback;

	public:

		// Constructor
		MockTrigger() = delete;

		inline MockTrigger(bool allow, bool continueExec, std::function<void()> callback):
		allowAction(allow), continueExecution(continueExec), executeCallback(callback) {}

		/*
		   Executes the EventTrigger.

		   Input:
		      The event that triggered this method call (Event &e)

		   Output:
		      A pair of flags that determine whether or not execution of event
		      listeners an their associated triggers should continue and
		      whether or not the action that triggered the event should be
		      allowed to continue or be suppressed (EventReturn)
		*/
		virtual trogdor::event::EventReturn operator()(trogdor::event::Event e);
};


#endif
