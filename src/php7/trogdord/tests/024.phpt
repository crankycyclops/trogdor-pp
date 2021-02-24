--TEST--
\Trogdord\Game\Dump\Slot::destroy()
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

		$dump = $game->dump();

		if (200 != $trogdord->status) {
			die('Game dump should be successful');
		}

		$game->dump();

		if (200 != $trogdord->status) {
			die('Second game dump should be successful');
		}

		$slots = $dump->slots();

		if (200 != $trogdord->status) {
			die('Getting dump slots should be successful');
		}

		if (2 != count($slots)) {
			die('There should be exactly two dump slots');
		}

		$slot = $dump->getSlot($slots[0]['slot']);

		if (200 != $trogdord->status) {
			die('Getting dump slot object should be successful');
		}

		$slot->destroy();

		if (200 != $trogdord->status) {
			die('Destroying dump slot object should be successful');
		}

		$slots = $dump->slots();

		if (1 != count($slots)) {
			die('After deletion, there should be exactly 1 slot left');
		}

		if ($slot->id == $slots[0]['slot']) {
			die('The wrong dump slot was destroyed');
		}

		// Make sure invalidated dump slot object's methods can't be called again
		try {
			$slot->destroy();
			die('Call should not be successful after dump slot has been destroyed and object has been invalidated.');
		} catch (\Trogdord\DumpSlotNotFound $e) {
			if ("Dump slot has already been destroyed" != $e->getMessage()) {
				die("restore(): Call to method on invalidated dump slot object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}

		// Clean up
		$game->destroy(true);
	}

	catch (Exception $e) {
		die("Caught instance of " . get_class($e) . ": " . $e->getMessage());
	}

	echo "done!";
?>
--EXPECT--
done!