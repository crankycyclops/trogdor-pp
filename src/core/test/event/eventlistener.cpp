#include <doctest.h>
#include <trogdor/event/eventlistener.h>

#include "../mock/mocktrigger.h"


TEST_SUITE("EventListener (event/eventlistener.cpp)") {

	TEST_CASE("EventListener (event/eventlistener.cpp): Sane construction") {

		trogdor::event::EventListener listener;
		CHECK(0 == listener.getTriggers().size());
	}

	TEST_CASE("EventListener (event/eventlistener.cpp): addTrigger() and getTriggers()") {

		trogdor::event::EventListener listener;

		std::unique_ptr<MockTrigger> trigger1 = std::make_unique<MockTrigger>(
			true, true, []{}
		);

		std::unique_ptr<MockTrigger> trigger2 = std::make_unique<MockTrigger>(
			true, true, []{}
		);

		MockTrigger *rawTriggerPtr1 = trigger1.get();
		MockTrigger *rawTriggerPtr2 = trigger2.get();

		listener.addTrigger("test", std::move(trigger1));
		listener.addTrigger("test", std::move(trigger2));

		auto &triggers = listener.getTriggers();
		CHECK(triggers.end() != triggers.find("test"));
		CHECK(2 == (*triggers.find("test")).second.size());
		CHECK(rawTriggerPtr1 == (*triggers.find("test")).second[0].get());
		CHECK(rawTriggerPtr2 == (*triggers.find("test")).second[1].get());
	}

	TEST_CASE("EventListener (event/eventlistener.cpp): dispatch()") {

		SUBCASE("Event listener with no triggers added") {

			// I don't have any assertions for this. Really, all we can test
			// here is that nothing crashes.
			trogdor::event::EventListener listener;
			listener.dispatch({"test", {&listener}, {}});
		}

		SUBCASE("Event listener with triggers, undefined event called") {

			bool triggerExecuted = false;
			trogdor::event::EventListener listener;

			listener.addTrigger("test", std::make_unique<MockTrigger>(
				true, true, [&]{
					triggerExecuted = true;
				}
			));

			listener.dispatch({"undefined", {&listener}, {}});
			CHECK(!triggerExecuted);
		}

		SUBCASE("Event listener with single trigger") {

			bool trigger1Executed = false;
			bool trigger2Executed = false;
			bool trigger3Executed = false;
			bool trigger4Executed = false;

			trogdor::event::EventListener listener1;
			trogdor::event::EventListener listener2;
			trogdor::event::EventListener listener3;
			trogdor::event::EventListener listener4;

			// Test every possible combination of allowAction / continueExecution
			listener1.addTrigger("test", std::make_unique<MockTrigger>(
				true, true, [&]{
					trigger1Executed = true;
				}
			));

			listener2.addTrigger("test", std::make_unique<MockTrigger>(
				true, false, [&]{
					trigger2Executed = true;
				}
			));

			listener3.addTrigger("test", std::make_unique<MockTrigger>(
				false, true, [&]{
					trigger3Executed = true;
				}
			));

			listener4.addTrigger("test", std::make_unique<MockTrigger>(
				false, false, [&]{
					trigger4Executed = true;
				}
			));

			auto result1 = listener1.dispatch({"test", {&listener1}, {}});
			auto result2 = listener2.dispatch({"test", {&listener2}, {}});
			auto result3 = listener3.dispatch({"test", {&listener3}, {}});
			auto result4 = listener4.dispatch({"test", {&listener4}, {}});

			CHECK(trigger1Executed);
			CHECK(trigger2Executed);
			CHECK(trigger3Executed);
			CHECK(trigger4Executed);

			CHECK(result1.allowAction);
			CHECK(result1.continueExecution);

			CHECK(result2.allowAction);
			CHECK(!result2.continueExecution);

			CHECK(!result3.allowAction);
			CHECK(result3.continueExecution);

			CHECK(!result4.allowAction);
			CHECK(!result4.continueExecution);
		}

		SUBCASE("First event allows the action and continues execution of remaining triggers") {

			bool trigger1Executed = false;
			bool trigger2Executed = false;

			trogdor::event::EventListener listener;

			listener.addTrigger("test", std::make_unique<MockTrigger>(
				true, true, [&]{
					trigger1Executed = true;
				}
			));

			listener.addTrigger("test", std::make_unique<MockTrigger>(
				true, true, [&]{
					trigger2Executed = true;
				}
			));

			auto result = listener.dispatch({"test", {&listener}, {}});

			CHECK(trigger1Executed);
			CHECK(trigger2Executed);

			CHECK(result.allowAction);
			CHECK(result.continueExecution);
		}

		SUBCASE("First event allows the action but discontinues execution of remaining triggers") {

			bool trigger1Executed = false;
			bool trigger2Executed = false;

			trogdor::event::EventListener listener;

			listener.addTrigger("test", std::make_unique<MockTrigger>(
				true, false, [&]{
					trigger1Executed = true;
				}
			));

			listener.addTrigger("test", std::make_unique<MockTrigger>(
				true, true, [&]{
					trigger2Executed = true;
				}
			));

			auto result = listener.dispatch({"test", {&listener}, {}});

			CHECK(trigger1Executed);
			CHECK(!trigger2Executed);

			CHECK(result.allowAction);
			CHECK(!result.continueExecution);
		}

		SUBCASE("First event disallows the action but continues execution of remaining triggers") {

			bool trigger1Executed = false;
			bool trigger2Executed = false;

			trogdor::event::EventListener listener;

			listener.addTrigger("test", std::make_unique<MockTrigger>(
				false, true, [&]{
					trigger1Executed = true;
				}
			));

			listener.addTrigger("test", std::make_unique<MockTrigger>(
				true, true, [&]{
					trigger2Executed = true;
				}
			));

			auto result = listener.dispatch({"test", {&listener}, {}});

			CHECK(trigger1Executed);
			CHECK(trigger2Executed);

			CHECK(!result.allowAction);
			CHECK(result.continueExecution);
		}

		SUBCASE("First event disallows the action and discontinues execution of remaining triggers") {

			bool trigger1Executed = false;
			bool trigger2Executed = false;

			trogdor::event::EventListener listener;

			listener.addTrigger("test", std::make_unique<MockTrigger>(
				false, false, [&]{
					trigger1Executed = true;
				}
			));

			listener.addTrigger("test", std::make_unique<MockTrigger>(
				true, true, [&]{
					trigger2Executed = true;
				}
			));

			auto result = listener.dispatch({"test", {&listener}, {}});

			CHECK(trigger1Executed);
			CHECK(!trigger2Executed);

			CHECK(!result.allowAction);
			CHECK(!result.continueExecution);
		}
	}

	TEST_CASE("EventListener (event/eventlistener.cpp): Copy construction") {

		// TODO
	}
}
