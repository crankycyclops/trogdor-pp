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

		CHECK(testResource->areIntegerAllocationsRequired());
		CHECK(testResource->getAmountAvailable());
		CHECK(10.0 == *testResource->getAmountAvailable());
		CHECK(testResource->getMaxAmountPerDepositor());
		CHECK(1.0 == *testResource->getMaxAmountPerDepositor());

		auto status = testResource->allocate(testRoom, 1);

		// Verify that the allocation succeeded
		CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == status);
		CHECK(1 == testResource->getDepositors().size());
		CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
			(*testResource->getDepositors().begin()).first.lock());
		CHECK(1 == (*testResource->getDepositors().begin()).second);
		CHECK(1 == testResource->getTotalAmountAllocated());

		// Copy construct a new resource based on the first and verify that it
		// doesn't copy over the depositors, since it's supposed to be a new
		// unique resource modeled after the original
		std::shared_ptr<trogdor::entity::Resource> testResourceCopy =
		std::make_shared<trogdor::entity::Resource>(
			*testResource,
			"gold copy"
		);

		CHECK(testResourceCopy->areIntegerAllocationsRequired());
		CHECK(testResourceCopy->getAmountAvailable());
		CHECK(10.0 == *testResourceCopy->getAmountAvailable());
		CHECK(testResourceCopy->getMaxAmountPerDepositor());
		CHECK(1.0 == *testResourceCopy->getMaxAmountPerDepositor());
		CHECK(0 == testResourceCopy->getDepositors().size());
		CHECK(0 == testResourceCopy->getTotalAmountAllocated());
	}

	TEST_CASE("Resource (entities/resource.cpp): allocate() and free()") {

		SUBCASE("Unlimited amount available, no allocation limit per entity, and integer allocations not required") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Resource> testResource =
			std::make_shared<trogdor::entity::Resource>(
				&mockGame,
				"gold"
			);

			std::shared_ptr<trogdor::entity::Room> testRoom =
			std::make_shared<trogdor::entity::Room>(
				&mockGame,
				"start",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			std::shared_ptr<trogdor::entity::Room> testRoom2 =
			std::make_shared<trogdor::entity::Room>(
				&mockGame,
				"cave",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			auto status = testResource->allocate(testRoom, 1);

			// Verify that the allocation succeeded and that the amount is correct
			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == status);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(1 == (*testResource->getDepositors().begin()).second);
			CHECK(1 == (*testRoom->getResources().begin()).second);
			CHECK(1 == testResource->getTotalAmountAllocated());

			status = testResource->allocate(testRoom, 2);

			// Verify that the second allocation succeeded and that it's
			// accounted for properly
			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == status);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(3 == (*testResource->getDepositors().begin()).second);
			CHECK(3 == (*testRoom->getResources().begin()).second);
			CHECK(3 == testResource->getTotalAmountAllocated());

			// Verify that fractional allocations work
			status = testResource->allocate(testRoom2, 0.5);

			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == status);
			CHECK(2 == testResource->getDepositors().size());
			CHECK(3.5 == testResource->getTotalAmountAllocated());

			double totalAmounts = 0;

			for (const auto &depositor: testResource->getDepositors()) {
				totalAmounts += depositor.second;
			}

			CHECK(3.5 == totalAmounts);

			// Allocation of zero amount should fail and existing
			// allocations should remain the same
			status = testResource->allocate(testRoom2, 0);

			CHECK(trogdor::entity::Resource::ALLOCATE_ZERO_OR_NEGATIVE_AMOUNT == status);
			CHECK(3.5 == testResource->getTotalAmountAllocated());

			totalAmounts = 0;

			for (const auto &depositor: testResource->getDepositors()) {
				totalAmounts += depositor.second;
			}

			CHECK(3.5 == totalAmounts);

			// Allocation of negative amount should fail and existing
			// allocations should remain the same
			status = testResource->allocate(testRoom2, -1);

			CHECK(trogdor::entity::Resource::ALLOCATE_ZERO_OR_NEGATIVE_AMOUNT == status);
			CHECK(3.5 == testResource->getTotalAmountAllocated());

			totalAmounts = 0;

			for (const auto &depositor: testResource->getDepositors()) {
				totalAmounts += depositor.second;
			}

			CHECK(3.5 == totalAmounts);

			// Calling free without a value results in testRoom2's entire
			// amount being deallocated (this is the same as passing 0, which
			// is the default value)
			auto statusFree = testResource->free(testRoom2);

			// Verify that testRoom2's entire allocation is gone
			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == statusFree);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(3 == (*testResource->getDepositors().begin()).second);
			CHECK(3 == (*testRoom->getResources().begin()).second);
			CHECK(3 == testResource->getTotalAmountAllocated());

			// Trying to free a negative value should fail
			statusFree = testResource->free(testRoom, -1);

			// Verify that nothing's changed
			CHECK(trogdor::entity::Resource::FREE_NEGATIVE_VALUE == statusFree);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(3 == (*testResource->getDepositors().begin()).second);
			CHECK(3 == (*testRoom->getResources().begin()).second);
			CHECK(3 == testResource->getTotalAmountAllocated());

			// Attempting to free more of a resource than an entity currently
			// holds should fail
			statusFree = testResource->free(testRoom, 100);

			// Verify that nothing's changed
			CHECK(trogdor::entity::Resource::FREE_EXCEEDS_ALLOCATION == statusFree);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(3 == (*testResource->getDepositors().begin()).second);
			CHECK(3 == (*testRoom->getResources().begin()).second);
			CHECK(3 == testResource->getTotalAmountAllocated());

			// Calling resource->free() for a certain amount of the allocated
			// resource should successfully free that specific amount from
			// the tangible entity's allocation
			statusFree = testResource->free(testRoom, 1);

			// Verify that the entity's allocation has been updated, along
			// with the resource's aggregate total
			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == statusFree);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(2 == (*testResource->getDepositors().begin()).second);
			CHECK(2 == (*testRoom->getResources().begin()).second);
			CHECK(2 == testResource->getTotalAmountAllocated());

			// Calling resource->free(entity, 0) with the 0 passed in
			// explicitly instead of as a default parameter should still
			// free the entity's entire allocation.
			statusFree = testResource->free(testRoom, 0);

			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == statusFree);
			CHECK(0 == testResource->getDepositors().size());
			CHECK(0 == testRoom->getResources().size());
			CHECK(0 == testResource->getTotalAmountAllocated());
		}

		SUBCASE("Unlimited amount available and no allocation limit per entity, but integer allocation is required") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Resource> testResource =
			std::make_shared<trogdor::entity::Resource>(
				&mockGame,
				"gold",
				std::nullopt,
				std::nullopt,
				true
			);

			std::shared_ptr<trogdor::entity::Room> testRoom =
			std::make_shared<trogdor::entity::Room>(
				&mockGame,
				"start",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			CHECK(true == testResource->areIntegerAllocationsRequired());
			CHECK(!testResource->getAmountAvailable());
			CHECK(!testResource->getMaxAmountPerDepositor());

			// Verify that amounts with fractional values are not allowed and
			// don't modify any current allocations
			auto status = testResource->allocate(testRoom, 1.5);

			CHECK(trogdor::entity::Resource::ALLOCATE_INT_REQUIRED == status);
			CHECK(0 == testResource->getDepositors().size());
			CHECK(0 == testRoom->getResources().size());
			CHECK(0 == testResource->getTotalAmountAllocated());

			// Now, verify that integral allocations succeed
			status = testResource->allocate(testRoom, 1);

			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == status);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(1 == (*testResource->getDepositors().begin()).second);
			CHECK(1 == (*testRoom->getResources().begin()).second);
			CHECK(1 == testResource->getTotalAmountAllocated());

			// Verify that amounts with fractional values cannot be freed
			// and that the current alloctions aren't modified
			auto statusFree = testResource->free(testRoom, 0.5);

			CHECK(trogdor::entity::Resource::FREE_INT_REQUIRED == statusFree);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(1 == (*testResource->getDepositors().begin()).second);
			CHECK(1 == (*testRoom->getResources().begin()).second);
			CHECK(1 == testResource->getTotalAmountAllocated());

			// Finally, test that ingegral free succeeds
			statusFree = testResource->free(testRoom, 1);

			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == statusFree);
			CHECK(0 == testResource->getDepositors().size());
			CHECK(0 == testRoom->getResources().size());
			CHECK(0 == testResource->getTotalAmountAllocated());
		}

		SUBCASE("Unlimited amount available and integer allocation not required, but there's an allocation limit per entity") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Resource> testResource =
			std::make_shared<trogdor::entity::Resource>(
				&mockGame,
				"gold",
				std::nullopt,
				1.0
			);

			std::shared_ptr<trogdor::entity::Room> testRoom =
			std::make_shared<trogdor::entity::Room>(
				&mockGame,
				"start",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			CHECK(!testResource->areIntegerAllocationsRequired());
			CHECK(!testResource->getAmountAvailable());
			CHECK(1.0 == testResource->getMaxAmountPerDepositor());

			// Verify that I can't allocate more than the maximum amount per
			// entity
			auto status = testResource->allocate(testRoom, 2);

			CHECK(trogdor::entity::Resource::ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED == status);
			CHECK(0 == testResource->getDepositors().size());
			CHECK(0 == testRoom->getResources().size());
			CHECK(0 == testResource->getTotalAmountAllocated());

			// This allocation should succeed
			status = testResource->allocate(testRoom, 1);

			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == status);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(1 == (*testResource->getDepositors().begin()).second);
			CHECK(1 == (*testRoom->getResources().begin()).second);
			CHECK(1 == testResource->getTotalAmountAllocated());

			// I shouldn't be able to allocate anymore, even if the value
			// requested itself is at or below the maximum
			status = testResource->allocate(testRoom, 0.5);

			CHECK(trogdor::entity::Resource::ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED == status);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(1 == (*testResource->getDepositors().begin()).second);
			CHECK(1 == (*testRoom->getResources().begin()).second);
			CHECK(1 == testResource->getTotalAmountAllocated());

			auto statusFree = testResource->free(testRoom);

			CHECK(trogdor::entity::Resource::ALLOCATE_OR_FREE_SUCCESS == statusFree);
			CHECK(0 == testResource->getDepositors().size());
			CHECK(0 == testRoom->getResources().size());
			CHECK(0 == testResource->getTotalAmountAllocated());
		}

		SUBCASE("Finite amount available") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Resource> testResource =
			std::make_shared<trogdor::entity::Resource>(
				&mockGame,
				"gold",
				5.0
			);

			std::shared_ptr<trogdor::entity::Room> testRoom =
			std::make_shared<trogdor::entity::Room>(
				&mockGame,
				"start",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			std::shared_ptr<trogdor::entity::Room> testRoom2 =
			std::make_shared<trogdor::entity::Room>(
				&mockGame,
				"cave",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			CHECK(!testResource->areIntegerAllocationsRequired());
			CHECK(5 == *testResource->getAmountAvailable());
			CHECK(!testResource->getMaxAmountPerDepositor());

			// Attempting to allocate more than the maximum amount available
			// should fail
			auto status = testResource->allocate(testRoom, 10);

			CHECK(trogdor::entity::Resource::ALLOCATE_TOTAL_AMOUNT_EXCEEDED == status);
			CHECK(0 == testResource->getDepositors().size());
			CHECK(0 == testRoom->getResources().size());
			CHECK(0 == testResource->getTotalAmountAllocated());

			// Try allocating the maximum amount to one entity, then attempt
			// to allocate more to another
			status = testResource->allocate(testRoom, 5);

			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(5 == (*testResource->getDepositors().begin()).second);
			CHECK(5 == (*testRoom->getResources().begin()).second);
			CHECK(5 == testResource->getTotalAmountAllocated());

			status = testResource->allocate(testRoom2, 1);

			CHECK(trogdor::entity::Resource::ALLOCATE_TOTAL_AMOUNT_EXCEEDED == status);
			CHECK(1 == testResource->getDepositors().size());
			CHECK(std::dynamic_pointer_cast<trogdor::entity::Tangible>(testRoom) ==
				(*testResource->getDepositors().begin()).first.lock());
			CHECK(5 == (*testResource->getDepositors().begin()).second);
			CHECK(5 == (*testRoom->getResources().begin()).second);
			CHECK(0 == testRoom2->getResources().size());
			CHECK(5 == testResource->getTotalAmountAllocated());
		}
	}
}
