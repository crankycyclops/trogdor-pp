--TEST--
\Trogdord\Game\Dump::destroy()
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

		if (!$dump instanceof \Trogdord\Game\Dump) {
			die("Dump appears to have been successful, but return result was not of type \Trogdord\Game\Dump even though it should have been.");
		}

		$fetchedDump = $trogdord->getDump($dump->id);

		if (200 != $trogdord->status) {
			die('Getting dump should have been successful');
		}

		if ($fetchedDump->id != $game->id) {
			die('Fetched dump and created dump ids should match but don\'t');
		}

		// Destroy the dump and confirm that it's properly invalidated
		$dump->destroy();

		if (200 != $trogdord->status) {
			die('Dump destruction should have been successful but was not');
		}

		try {
			$fetchedDump = $trogdord->getDump($dump->id);
			die('Dump should have been destroyed but was not');
		} catch (\Trogdord\GameNotFound $e) {}

		// Verify that we can't call dump() again once the object has been invalidated
		try {
			$dump->destroy();
		} catch (\Trogdord\GameNotFound $e) {
			if ("Dump has already been destroyed" != $e->getMessage()) {
				die("\Trogdord\Game\Dump::destroy(): Call to method on invalidated dump object resulted in incorrect message: '" . $e->getMessage() . "'. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
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