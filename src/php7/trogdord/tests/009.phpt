--TEST--
\Trogdord\Game::destroy()
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

	// We'll have to run this unit test twice, once with state enabled and once
	// with it disabled.
	try {

		$config = $trogdord->config();

		// Version of the test when the state feature is enabled
		if ($config['state.enabled']) {

			/////////////////////////////////////////////////////////////////////
			// 1. Test with no argument (should default to destroyDump = true) //
			/////////////////////////////////////////////////////////////////////

			// Requires sample game.xml to be installed under definitions path
			// configured in trogdord.ini.
			$game = $trogdord->newGame('My Game', 'game.xml');

			if (200 != $trogdord->status) {
				die('Game creation should have been successful.');
			}

			$id = $game->id;

			// Dump (and validate that the dump was successful)
			$game->dump();

			if (200 != $trogdord->status) {
				die('1: Game dump should be successful');
			}

			$dump = $trogdord->getDump($id);

			if (200 != $trogdord->status) {
				die('1: Dump should have been successful');
			} else if (!$dump instanceof \Trogdord\Game\Dump) {
				die('1: Game dump should be successful');
			}

			$game->destroy();

			if (200 != $trogdord->status) {
				die('1: destroying game should have been successful');
			}

			// Verify that the game no longer exists in memory
			try {
				$trogdord->getGame($id);
				die('1: Game should have been destroyed');
			} catch (\Trogdord\GameNotFound $e) {}

			// Verify that the dump was destroyed along with the game
			try {
				$dump = $trogdord->getDump($id);
				die('1: Game dump should have been destroyed');
			} catch (\Trogdord\GameNotFound $e) {}

			////////////////////////////////////////////////
			// 2. Test with destroyDump = true (explicit) //
			////////////////////////////////////////////////

			// Requires sample game.xml to be installed under definitions path
			// configured in trogdord.ini.
			$game = $trogdord->newGame('My Game', 'game.xml');

			if (200 != $trogdord->status) {
				die('Game creation should have been successful.');
			}

			$id = $game->id;

			// Dump (and validate that the dump was successful)
			$game->dump();

			if (200 != $trogdord->status) {
				die('2: Game dump should be successful');
			}

			$dump = $trogdord->getDump($id);

			if (200 != $trogdord->status) {
				die('2: Dump should have been successful');
			} else if (!$dump instanceof \Trogdord\Game\Dump) {
				die('2: Game dump should be successful');
			}

			$game->destroy(true);

			if (200 != $trogdord->status) {
				die('2: destroying game should have been successful');
			}

			// Verify that the game no longer exists in memory
			try {
				$trogdord->getGame($id);
				die('2: Game should have been destroyed');
			} catch (\Trogdord\GameNotFound $e) {}

			// Verify that the dump was destroyed along with the game
			try {
				$dump = $trogdord->getDump($id);
				die('2: Game dump should have been destroyed');
			} catch (\Trogdord\GameNotFound $e) {}

			//////////////////////////////////////
			// 3. Test with destroyDump = false //
			//////////////////////////////////////

			// Requires sample game.xml to be installed under definitions path
			// configured in trogdord.ini.
			$game = $trogdord->newGame('My Game', 'game.xml');

			if (200 != $trogdord->status) {
				die('Game creation should have been successful.');
			}

			$id = $game->id;

			// Dump (and validate that the dump was successful)
			$game->dump();

			if (200 != $trogdord->status) {
				die('3: Game dump should be successful');
			}

			$dump = $trogdord->getDump($id);

			if (200 != $trogdord->status) {
				die('3: Dump should have been successful');
			} else if (!$dump instanceof \Trogdord\Game\Dump) {
				die('3: Game dump should be successful');
			}

			$game->destroy(false);

			if (200 != $trogdord->status) {
				die('3: destroying game should have been successful');
			}

			// Verify that the game no longer exists in memory
			try {
				$trogdord->getGame($id);
				die('3: Game should have been destroyed');
			} catch (\Trogdord\GameNotFound $e) {}

			// Verify that the dump was NOT destroyed
			try {
				$dump = $trogdord->getDump($id);
			} catch (\Trogdord\GameNotFound $e) {
				die('3: Game dump should NOT have been destroyed');
			}

			// Clean up the dump that wasn't destroyed
			$dump->destroy();

			if (200 != $trogdord->status) {
				die('Destroying dump should have been successful');
			}
		}

		// Version of the test when state is disabled. The three versions
		// of the call should all do the same thing.
		else {

			/////////////////////////////////////////////////////////////////////
			// 1. Test with no argument (should default to destroyDump = true) //
			/////////////////////////////////////////////////////////////////////

			// Requires sample game.xml to be installed under definitions path
			// configured in trogdord.ini.
			$game = $trogdord->newGame('My Game', 'game.xml');

			if (200 != $trogdord->status) {
				die('Game creation should have been successful.');
			}

			$id = $game->id;

			$game->destroy();

			if (200 != $trogdord->status) {
				die('1: destroying game should have been successful');
			}

			// Verify that the game no longer exists in memory
			try {
				$trogdord->getGame($id);
				die('1: Game should have been destroyed');
			} catch (\Trogdord\GameNotFound $e) {}

			////////////////////////////////////////////////
			// 2. Test with destroyDump = true (explicit) //
			////////////////////////////////////////////////

			// Requires sample game.xml to be installed under definitions path
			// configured in trogdord.ini.
			$game = $trogdord->newGame('My Game', 'game.xml');

			if (200 != $trogdord->status) {
				die('Game creation should have been successful.');
			}

			$id = $game->id;

			$game->destroy(true);

			if (200 != $trogdord->status) {
				die('1: destroying game should have been successful');
			}

			// Verify that the game no longer exists in memory
			try {
				$trogdord->getGame($id);
				die('1: Game should have been destroyed');
			} catch (\Trogdord\GameNotFound $e) {}

			//////////////////////////////////////
			// 3. Test with destroyDump = false //
			//////////////////////////////////////

			// Requires sample game.xml to be installed under definitions path
			// configured in trogdord.ini.
			$game = $trogdord->newGame('My Game', 'game.xml');

			if (200 != $trogdord->status) {
				die('Game creation should have been successful.');
			}

			$id = $game->id;

			$game->destroy(false);

			if (200 != $trogdord->status) {
				die('1: destroying game should have been successful');
			}

			// Verify that the game no longer exists in memory
			try {
				$trogdord->getGame($id);
				die('1: Game should have been destroyed');
			} catch (\Trogdord\GameNotFound $e) {}
		}

		// Make sure invalidated game object's methods can't be called
		try {
			$game->destroy();
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("destroy(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
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