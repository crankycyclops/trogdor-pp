--TEST--
\Trogdord\Game\Dump::restore()
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

		// Make a second dump with a meta value set
		$game->setMeta(['second_slot' => 'yes']);

		$game->dump();

		if (200 != $trogdord->status) {
			die('Second game dump should be successful');
		}

		// Destroy the game without destroying the dump
		$game->destroy(false);

		if (200 != $trogdord->status) {
			die('Destroying game should be successful');
		}

		try {
			$trogdord->getGame($game->id);
			die('The game should no longer exists but does.');
		} catch (\Trogdord\GameNotFound $e) {}

		$slots = $dump->slots();

		if (200 != $trogdord->status) {
			die('Getting dump slots should be successful');
		}

		// I don't think trogdord guarantees that the slots will be in order
		usort($slots, function($a, $b) {
			return $a['slot'] == $b['slot'] ? 0 : ($a['slot'] > $b['slot'] ? -1 : 1);
		});

		// Test slot that doesn't exist
		try {
			$dump->restore($slots[0]['slot'] + 5);
		} catch (\Trogdord\GameNotFound $e) {}

		// Test the least recent slot, which doesn't have the meta value set
		$game = $dump->restore($slots[1]['slot']);

		if (200 != $trogdord->status) {
			die('Getting dump slots should be successful');
		}

		if ('' != $game->getMeta(['second_slot'])['second_slot']) {
			die('The least recent slot should have been restored but was not');
		}

		// Destroy the game without destroying the dump
		$game->destroy(false);

		if (200 != $trogdord->status) {
			die('Destroying game should be successful');
		}

		// Test the default slot, which should be the most recent
		$game = $dump->restore();

		if (200 != $trogdord->status) {
			die('Destroying game should be successful');
		}

		if ('yes' != $game->getMeta(['second_slot'])['second_slot']) {
			die('The most recent slot should have been restored but was not');
		}

		// Clean up
		$dump->destroy();

		if (200 != $trogdord->status) {
			die('Destroying dump should have been successful');
		}

		// Make sure invalidated dump object's methods can't be called again
		try {
			$dump->restore();
			die('Call should not be successful after dump has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Dump has already been destroyed" != $e->getMessage()) {
				die("restore(): Call to method on invalidated dump object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
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