--TEST--
\Trogdord\Game::dump()
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

		// Version of the test when the state feature is enabled.
		if ($config['state.enabled']) {

			// Requires sample game.xml to be installed under definitions path
			// configured in trogdord.ini.
			$game = $trogdord->newGame('My Game', 'game.xml');

			if (200 != $trogdord->status) {
				die('Game creation should have been successful.');
			}

			$id = $game->id;

			// Dump (and validate that the dump was successful)
			$dump = $game->dump();

			if (200 != $trogdord->status) {
				die('Game dump should be successful');
			}

			if (!$dump instanceof \Trogdord\Game\Dump) {
				die('Return value of $game->dump() should be of type \Trogdord\GameDump but isn\'t');
			}

			$dumpCopy = $trogdord->getDump($id);

			// Verify that the result of $trogdord->getDump() and the return value
			// of $game->dump() are both objects pointing to the same resource.
			if ($dumpCopy->id != $dump->id) {
				die('Return value of either $game->dump() or $trogdord->getDump($id) is not correct!');
			}

			if (200 != $trogdord->status) {
				die('Dump should have been successful');
			} else if (!$dump instanceof \Trogdord\Game\Dump) {
				die('Game dump should be successful');
			}
		}

		// Version of the test when state is disabled.
		else {

			// Requires sample game.xml to be installed under definitions path
			// configured in trogdord.ini.
			$game = $trogdord->newGame('My Game', 'game.xml');

			if (200 != $trogdord->status) {
				die('Game creation should have been successful.');
			}

			try {
				$game->dump();
				die('Call to $game->dump() should not be successful when state is disabled.');
			}

			catch (\Trogdord\RequestException $e) {
				if (501 != $e->getCode()) {
					die('Should have received a 501 operation not supported status');
				}
			}

			if (501 != $trogdord->status) {
				die('$trogdord->status was not properly set after unsuccessful call to $game->dump()');
			}
		}

		// Clean up
		$game->destroy();

		if (200 != $trogdord->status) {
			die('Destroying game should have been successful');
		}

		// Make sure invalidated game object's methods can't be called again
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