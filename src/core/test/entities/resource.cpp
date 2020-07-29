#include <doctest.h>

#include <trogdor/entities/room.h>
#include <trogdor/entities/resource.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


// TODO: make sure to verify a test case where allocations have been made, and
// then we copy and verify that those allocations/depositors don't exist in the
// copy
TEST_SUITE("Resource (entities/resource.cpp)") {

	TEST_CASE("Resource (entities/resource.cpp): Sane Construction with defaults") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Resource testResource(
			&mockGame,
			"gold"
		);

		CHECK(!testResource.areIntegerAllocationsRequired());
		CHECK(!testResource.getAmountAvailable());
		CHECK(!testResource.getMaxAmountPerDepositor());
		CHECK(0.0 == testResource.getTotalAmountAllocated());
		CHECK(0 == testResource.getDepositors().size());
	}

	TEST_CASE("Resource (entities/resource.cpp): Sane Construction with custom amount available") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Resource testResource(
			&mockGame,
			"gold",
			10.5
		);

		CHECK(!testResource.areIntegerAllocationsRequired());
		CHECK(testResource.getAmountAvailable());
		CHECK(10.5 == *testResource.getAmountAvailable());
		CHECK(!testResource.getMaxAmountPerDepositor());
		CHECK(0.0 == testResource.getTotalAmountAllocated());
		CHECK(0 == testResource.getDepositors().size());
	}

	TEST_CASE("Resource (entities/resource.cpp): Sane Construction with custom amount available and max amount per depositor") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Resource testResource(
			&mockGame,
			"gold",
			10.5,
			1.0
		);

		CHECK(!testResource.areIntegerAllocationsRequired());
		CHECK(testResource.getAmountAvailable());
		CHECK(10.5 == *testResource.getAmountAvailable());
		CHECK(testResource.getMaxAmountPerDepositor());
		CHECK(1.0 == *testResource.getMaxAmountPerDepositor());
		CHECK(0.0 == testResource.getTotalAmountAllocated());
		CHECK(0 == testResource.getDepositors().size());
	}

	TEST_CASE("Resource (entities/resource.cpp): Sane Construction with custom amount available, max amount per depositor, and integer-only allocations turned on") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Resource testResource(
			&mockGame,
			"gold",
			10.0,
			1.0,
			true
		);

		CHECK(testResource.areIntegerAllocationsRequired());
		CHECK(testResource.getAmountAvailable());
		CHECK(10.0 == *testResource.getAmountAvailable());
		CHECK(testResource.getMaxAmountPerDepositor());
		CHECK(1.0 == *testResource.getMaxAmountPerDepositor());
		CHECK(0.0 == testResource.getTotalAmountAllocated());
		CHECK(0 == testResource.getDepositors().size());
	}

	TEST_CASE("Resource (entities/resource.cpp): Sane Copy Construction") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Resource> testResource =
		std::make_shared<trogdor::entity::Resource>(
			&mockGame,
			"gold",
			10.0,
			1,
			true
		);

		std::shared_ptr<trogdor::entity::Room> testRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"start",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		testRoom->getShared();

		CHECK(testResource->areIntegerAllocationsRequired());
		CHECK(testResource->getAmountAvailable());
		CHECK(10.0 == *testResource->getAmountAvailable());
		CHECK(testResource->getMaxAmountPerDepositor());
		CHECK(1.0 == *testResource->getMaxAmountPerDepositor());

		testResource->allocate(testRoom, 1);

		// Verify that the allocation succeeded
		CHECK(1 == testResource->getDepositors().size());
		CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
			(*testResource->getDepositors().begin()).first.lock());
		CHECK(1 == (*testResource->getDepositors().begin()).second);

		// Copy construct a new resource based on the first and verify that it
		// doesn't copy over the depositors, since it's supposed to be a new
		// unique resource modeled after the original
		std::shared_ptr<trogdor::entity::Resource> testResourceCopy =
		std::make_shared<trogdor::entity::Resource>(
			*testResource,
			"gold_copy"
		);

		CHECK(testResourceCopy->areIntegerAllocationsRequired());
		CHECK(testResourceCopy->getAmountAvailable());
		CHECK(10.0 == *testResourceCopy->getAmountAvailable());
		CHECK(testResourceCopy->getMaxAmountPerDepositor());
		CHECK(1.0 == *testResourceCopy->getMaxAmountPerDepositor());
		CHECK(0 == testResourceCopy->getDepositors().size());
	}
}
