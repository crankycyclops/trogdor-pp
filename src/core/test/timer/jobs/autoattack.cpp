#include <doctest.h>

#include <trogdor/timer/jobs/autoattack.h>

#include <trogdor/entities/room.h>
#include <trogdor/entities/creature.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


TEST_SUITE("Autoattack Timer Job (timer/jobs/autoattack.cpp)") {

	TEST_CASE("Autoattack Timer Job (timer/jobs/autoattack.cpp): Attacker is dead") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Room> mockRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"start",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> attacker =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"attacker",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> defender =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"defender",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		mockRoom->insertThing(attacker);
		mockRoom->insertThing(defender);

		// Until we set a max health, a Being is considered immortal
		attacker->setProperty(trogdor::entity::Being::HealthProperty, 10);
		attacker->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::HealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);

		// RIP, random attacker!
		attacker->die();

		trogdor::Timer mockTimer(&mockGame);
		trogdor::AutoAttackTimerJob job(&mockGame, 1, -1, 1, attacker.get(), defender.get());

		job.execute();

		// If the attacker is dead, setExecutions(0) will be called,
		// indicating that the job should no longer be executed and that it
		// should be removed on the next timer tick.
		CHECK(0 == job.getExecutions());
	}

	TEST_CASE("Autoattack Timer Job (timer/jobs/autoattack.cpp): Defender is dead") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Room> mockRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"start",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> attacker =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"attacker",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> defender =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"defender",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		mockRoom->insertThing(attacker);
		mockRoom->insertThing(defender);

		// Until we set a max health, a Being is considered immortal
		attacker->setProperty(trogdor::entity::Being::HealthProperty, 10);
		attacker->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::HealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);

		defender->die();

		trogdor::Timer mockTimer(&mockGame);
		trogdor::AutoAttackTimerJob job(&mockGame, 1, -1, 1, attacker.get(), defender.get());

		job.execute();

		// If the attacker is dead, setExecutions(0) will be called,
		// indicating that the job should no longer be executed and that it
		// should be removed on the next timer tick.
		CHECK(0 == job.getExecutions());
	}

	TEST_CASE("Autoattack Timer Job (timer/jobs/autoattack.cpp): Attacker and Defender are dead") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Room> mockRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"start",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> attacker =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"attacker",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> defender =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"defender",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		mockRoom->insertThing(attacker);
		mockRoom->insertThing(defender);

		// Until we set a max health, a Being is considered immortal
		attacker->setProperty(trogdor::entity::Being::HealthProperty, 10);
		attacker->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::HealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);

		attacker->die();
		defender->die();

		trogdor::Timer mockTimer(&mockGame);
		trogdor::AutoAttackTimerJob job(&mockGame, 1, -1, 1, attacker.get(), defender.get());

		job.execute();

		// If the attacker is dead, setExecutions(0) will be called,
		// indicating that the job should no longer be executed and that it
		// should be removed on the next timer tick.
		CHECK(0 == job.getExecutions());
	}

	TEST_CASE("Autoattack Timer Job (timer/jobs/autoattack.cpp): Defender isn't attackable") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Room> mockRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"start",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> attacker =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"attacker",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> defender =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"defender",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		mockRoom->insertThing(attacker);
		mockRoom->insertThing(defender);

		// Until we set a max health, a Being is considered immortal
		attacker->setProperty(trogdor::entity::Being::HealthProperty, 10);
		attacker->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::HealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);

		defender->removeTag(trogdor::entity::Being::AttackableTag);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::AutoAttackTimerJob job(&mockGame, 1, -1, 1, attacker.get(), defender.get());

		job.execute();

		// If the attacker is dead, setExecutions(0) will be called,
		// indicating that the job should no longer be executed and that it
		// should be removed on the next timer tick.
		CHECK(0 == job.getExecutions());
	}

	TEST_CASE("Autoattack Timer Job (timer/jobs/autoattack.cpp): Defender isn't attackable") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Room> attackersRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"attackersRoom",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Room> defendersRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"defendersRoom",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> attacker =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"attacker",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> defender =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"defender",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		attackersRoom->insertThing(attacker);
		defendersRoom->insertThing(defender);

		// Until we set a max health, a Being is considered immortal
		attacker->setProperty(trogdor::entity::Being::HealthProperty, 10);
		attacker->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::HealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::AutoAttackTimerJob job(&mockGame, 1, -1, 1, attacker.get(), defender.get());

		job.execute();

		// If the attacker is dead, setExecutions(0) will be called,
		// indicating that the job should no longer be executed and that it
		// should be removed on the next timer tick.
		CHECK(0 == job.getExecutions());
	}

	TEST_CASE("Autoattack Timer Job (timer/jobs/autoattack.cpp): attacker->attack() should be called") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Room> mockRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"attackersRoom",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> attacker =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"attacker",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> defender =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"defender",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		mockRoom->insertThing(attacker);
		mockRoom->insertThing(defender);

		// Until we set a max health, a Being is considered immortal
		attacker->setProperty(trogdor::entity::Being::HealthProperty, 10);
		attacker->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::HealthProperty, 10);
		defender->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);

		// Make absolutely certain the defender is attackable (right now this
		// is the default, but that might change in the future)
		defender->setTag(trogdor::entity::Being::AttackableTag);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::AutoAttackTimerJob job(&mockGame, 1, -1, 1, attacker.get(), defender.get());

		job.execute();

		// TODO: this isn't enough. I need to write Event Handler mocking that
		// I can use to verify that attacker->attack() was actually called.
		CHECK(-1 == job.getExecutions());
	}
}
