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