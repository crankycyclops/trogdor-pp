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

		// The event trigger's name. Used for type comparison.
		static constexpr const char *CLASS_NAME = "MockTrigger";

		// Constructor
		MockTrigger() = delete;

		inline MockTrigger(bool allow, bool continueExec, std::function<void()> callback):
		allowAction(allow), continueExecution(continueExec), executeCallback(callback) {}

         /*
            Returns the class name.

            Input:
               (none)

            Output:
               Class name (const char *)
         */
         virtual const char *getClassName();

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

		/*
		   Returns a serialized version of the EventTrigger instance.

		   Input:
		      (none)

		   Output:
		      Serialized instance of EventTrigger (std::shared_ptr<serial::Serializable>)
		*/
		virtual std::shared_ptr<trogdor::serial::Serializable> serialize();
};


#endif
