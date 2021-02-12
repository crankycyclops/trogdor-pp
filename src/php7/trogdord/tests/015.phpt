--TEST--
Verify Entity Class Hierarchy
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
<?php require_once('inc/skipifconnectfailure.inc'); ?>
--FILE--
<?php

	require_once("inc/connect.inc");
	$trogdord = trogConnect();

	if (!$trogdord instanceof \Trogdord) {
		die($trogdord->getMessage());
	}

	try {

		// Requires sample game.xml to be installed under definitions path
		// configured in trogdord.ini.
		$game = $trogdord->newGame('My Game', 'game.xml');

		if (200 != $trogdord->status) {
			die('Creating game should have been successful');
		}

		//////////////////////////////////////////////////////////
		// Verify class hierarchy for each complete entity type //
		//////////////////////////////////////////////////////////

		// Since entities can't be instantiated directly, I must get/creature
		// objects using the game object.
		$player = $game->createPlayer("player");

		if (200 != $trogdord->status) {
			die('Creating player should have been successful');
		}

		if (!$player instanceof \Trogdord\Player) {
			die('$player should be an instance of \Trogdord\Player but is not');
		}

		if (!$player instanceof \Trogdord\Being) {
			die('$player should inherit from \Trogdord\Being but doesn\'t');
		}

		if (!$player instanceof \Trogdord\Thing) {
			die('$player should inherit from \Trogdord\Thing but doesn\'t');
		}

		if (!$player instanceof \Trogdord\Tangible) {
			die('$player should inherit from \Trogdord\Tangible but doesn\'t');
		}

		if (!$player instanceof \Trogdord\Entity) {
			die('$player should inherit from \Trogdord\Entity but doesn\'t');
		}

		// I'll also make sure it doesn't inherit from the wrong classes
		if ($player instanceof \Trogdord\Creature) {
			die('$player should NOT inherit from \Trogdord\Creature but does');
		}

		if ($player instanceof \Trogdord\Object) {
			die('$player should NOT inherit from \Trogdord\Object but does');
		}

		if ($player instanceof \Trogdord\Resource) {
			die('$player should NOT inherit from \Trogdord\Resource but does');
		}

		if ($player instanceof \Trogdord\Place) {
			die('$player should NOT inherit from \Trogdord\Place but does');
		}

		if ($player instanceof \Trogdord\Room) {
			die('$player should NOT inherit from \Trogdord\Room but does');
		}

		//////

		$creature = $game->getCreature("trogdor");

		if (200 != $trogdord->status) {
			die('Getting creature should have been successful');
		}

		if (!$creature instanceof \Trogdord\Creature) {
			die('$creature should be an instance of \Trogdord\Creature but is not');
		}

		if (!$creature instanceof \Trogdord\Being) {
			die('$creature should inherit from \Trogdord\Being but doesn\'t');
		}

		if (!$creature instanceof \Trogdord\Thing) {
			die('$creature should inherit from \Trogdord\Thing but doesn\'t');
		}

		if (!$creature instanceof \Trogdord\Tangible) {
			die('$creature should inherit from \Trogdord\Tangible but doesn\'t');
		}

		if (!$creature instanceof \Trogdord\Entity) {
			die('$creature should inherit from \Trogdord\Entity but doesn\'t');
		}

		// I'll also make sure it doesn't inherit from the wrong classes
		if ($creature instanceof \Trogdord\Player) {
			die('$creature should NOT inherit from \Trogdord\Player but does');
		}

		if ($creature instanceof \Trogdord\Object) {
			die('$creature should NOT inherit from \Trogdord\Object but does');
		}

		if ($creature instanceof \Trogdord\Resource) {
			die('$creature should NOT inherit from \Trogdord\Resource but does');
		}

		if ($creature instanceof \Trogdord\Place) {
			die('$creature should NOT inherit from \Trogdord\Place but does');
		}

		if ($creature instanceof \Trogdord\Room) {
			die('$creature should NOT inherit from \Trogdord\Room but does');
		}

		//////

		$room = $game->getRoom("start");

		if (200 != $trogdord->status) {
			die('Getting creature should have been successful');
		}

		if (!$room instanceof \Trogdord\Room) {
			die('$room should be an instance of \Trogdord\Room but is not');
		}

		if (!$room instanceof \Trogdord\Place) {
			die('$room should inherit from \Trogdord\Place but doesn\'t');
		}

		if (!$room instanceof \Trogdord\Tangible) {
			die('$room should inherit from \Trogdord\Tangible but doesn\'t');
		}

		if (!$room instanceof \Trogdord\Entity) {
			die('$room should inherit from \Trogdord\Entity but doesn\'t');
		}

		// I'll also make sure it doesn't inherit from the wrong classes
		if ($room instanceof \Trogdord\Player) {
			die('$room should NOT be an instance of \Trogdord\Player but does');
		}

		if ($room instanceof \Trogdord\Creature) {
			die('$room should be an instance of \Trogdord\Creature but does');
		}

		if ($room instanceof \Trogdord\Object) {
			die('$room should NOT inherit from \Trogdord\Object but does');
		}

		if ($room instanceof \Trogdord\Resource) {
			die('$room should NOT inherit from \Trogdord\Resource but does');
		}

		if ($room instanceof \Trogdord\Thing) {
			die('$room should inherit from \Trogdord\Thing but does');
		}

		if ($room instanceof \Trogdord\Being) {
			die('$room should inherit from \Trogdord\Being but does');
		}

		//////

		$object = $game->getObject("candle");

		if (200 != $trogdord->status) {
			die('Getting creature should have been successful');
		}

		if (!$object instanceof \Trogdord\Object) {
			die('$object should be an instance of \Trogdord\Object but is not');
		}

		if (!$object instanceof \Trogdord\Thing) {
			die('$object should inherit from \Trogdord\Thing but doesn\'t');
		}

		if (!$object instanceof \Trogdord\Tangible) {
			die('$object should inherit from \Trogdord\Tangible but doesn\'t');
		}

		if (!$object instanceof \Trogdord\Entity) {
			die('$object should inherit from \Trogdord\Entity but doesn\'t');
		}

		// I'll also make sure it doesn't inherit from the wrong classes
		if ($object instanceof \Trogdord\Resource) {
			die('$object should NOT inherit from \Trogdord\Resource but does');
		}

		if ($object instanceof \Trogdord\Being) {
			die('$room should inherit from \Trogdord\Being but does');
		}

		// I'll also make sure it doesn't inherit from the wrong classes
		if ($object instanceof \Trogdord\Player) {
			die('$object should NOT be an instance of \Trogdord\Player but does');
		}

		if ($object instanceof \Trogdord\Creature) {
			die('$object should be an instance of \Trogdord\Creature but does');
		}

		if ($object instanceof \Trogdord\Place) {
			die('$object should NOT inherit from \Trogdord\Place but does');
		}

		if ($object instanceof \Trogdord\Room) {
			die('$object should NOT inherit from \Trogdord\Room but does');
		}

		//////

		$resource = $game->getResource("coin");

		if (200 != $trogdord->status) {
			die('Getting creature should have been successful');
		}

		if (!$resource instanceof \Trogdord\Resource) {
			die('$resource should be an instance of \Trogdord\Resource but is not');
		}

		if (!$resource instanceof \Trogdord\Entity) {
			die('$resource should inherit from \Trogdord\Entity but doesn\'t');
		}

		// I'll also make sure it doesn't inherit from the wrong classes
		if ($resource instanceof \Trogdord\Tangible) {
			die('$resource should NOT be an instance of \Trogdord\Tangible but does');
		}

		if ($resource instanceof \Trogdord\Place) {
			die('$resource should NOT be an instance of \Trogdord\Place but does');
		}

		if ($resource instanceof \Trogdord\Room) {
			die('$resource should NOT be an instance of \Trogdord\Room but does');
		}

		if ($resource instanceof \Trogdord\Thing) {
			die('$resource should NOT be an instance of \Trogdord\Thing but does');
		}

		if ($resource instanceof \Trogdord\Object) {
			die('$resource should NOT be an instance of \Trogdord\Object but does');
		}

		if ($resource instanceof \Trogdord\Being) {
			die('$resource should NOT be an instance of \Trogdord\Being but does');
		}

		if ($resource instanceof \Trogdord\Player) {
			die('$resource should NOT be an instance of \Trogdord\Player but does');
		}

		if ($resource instanceof \Trogdord\Creature) {
			die('$resource should NOT be an instance of \Trogdord\Creature but does');
		}

		//////

		// Clean up
		$game->destroy();

		if (200 != $trogdord->status) {
			die('Destroying the game should have been successful');
		}
	}

	catch (Exception $e) {
		die("Caught instance of " . get_class($e) . ": " . $e->getMessage());
	}

	echo "done!";
?>
--EXPECT--
done!