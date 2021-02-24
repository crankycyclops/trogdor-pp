--TEST--
\Trogdord\Game\Dump\Slot::restore()
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
			die('Game creation should have been successful.');
		}

		// Dump (and validate that the dump was successful)
		$dump = $game->dump();

		if (200 != $trogdord->status) {
			die('Game dump should be successful');
		}

		$slots = $dump->slots();

		if (200 != $trogdord->status) {
			die('Getting dump slots should be successful');
		}

		$slot = $dump->getSlot($slots[0]['slot']);

		if (200 != $trogdord->status) {
			die('Getting dump slot object should be successful');
		}

		// Destroy the game without destroying the dump
		$game->destroy(false);

		if (200 != $trogdord->status) {
			die('Destroying game should be successful');
		}

		$slot->restore();

		if (200 != $trogdord->status) {
			die('Restoring dump slot should be successful');
		}

		// Verify that we can get the game again
		$game = $trogdord->getGame($dump->id);

		if (200 != $trogdord->status) {
			die('Getting restored game should be successful');
		}

		// Clean up (destroying the only dump slot also causes the parent dump
		// to be destroyed)
		$slot->destroy();

		if (200 != $trogdord->status) {
			die('Destroying dump slot should have been successful');
		}

		// Make sure invalidated dump slot object's methods can't be called again
		try {
			$slot->restore();
			die('Call should not be successful after dump slot has been destroyed and object has been invalidated.');
		} catch (\Trogdord\DumpSlotNotFound $e) {
			if ("Dump slot has already been destroyed" != $e->getMessage()) {
				die("restore(): Call to method on invalidated dump slot object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}
	}

	catch (Exception $e) {
		die("Caught instance of " . get_class($e) . ": " . $e->getMessage());
	}

	echo "done!";
?>
--EXPECT--
done!