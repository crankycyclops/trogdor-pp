--TEST--
\Trogdord\Game\Dump::getSlot()
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
<?php require_once('inc/skipifstatedisabled.inc'); ?>
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

		$dump = $game->dump();

		if (200 != $game->trogdord->status) {
			die('Dumping game should have been successful');
		}

		$slots = $dump->slots();

		if (200 != $game->trogdord->status) {
			die('Getting game slots should have been successful');
		}

		$slot = $dump->getSlot($slots[0]['slot']);

		if (200 != $game->trogdord->status) {
			die('Getting game slot object should have been successful');
		}

		if (!$slot instanceof \Trogdord\Game\Dump\Slot) {
			die('$slot should be instance of \Trogdord\Game\Dump\Slot but is not');
		}

		// Destroy the dump and confirm that it's properly invalidated
		$dump->destroy();

		if (200 != $trogdord->status) {
			die('Dump destruction should have been successful but was not');
		}

		try {
			$dump->getSlot($slots[0]['slot']);
			die('Dump slot should have been destroyed but was not');
		} catch (\Trogdord\GameNotFound $e) {}

		// Verify that we can't call dump() again once the object has been invalidated
		try {
			$dump->getSlot($slots[0]['slot']);
		} catch (\Trogdord\GameNotFound $e) {
			if ("Dump has already been destroyed" != $e->getMessage()) {
				die("\Trogdord\Game\Dump::getSlot(): Call to method on invalidated dump object resulted in incorrect message: '" . $e->getMessage() . "'. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}

		// Cleanup
		$game->destroy();

		if (200 != $trogdord->status) {
			die('Game destruction should have been successful but was not');
		}
	}

	catch (Exception $e) {
		die("Caught instance of " . get_class($e) . ": " . $e->getMessage());
	}

	echo "done!";
?>
--EXPECT--
done!