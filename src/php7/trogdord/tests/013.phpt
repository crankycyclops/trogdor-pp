--TEST--
\Trogdord\Game::createPlayer()
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

		// We need at least one player to test all entity types
		$player = $game->createPlayer("player");

		if (200 != $trogdord->status) {
			die('Creating player in game should have been successful');
		}

		if (!$player instanceof \Trogdord\Player) {
			die('$player should be an instance of \Trogdord\Player');
		}

		// Clean up
		$game->destroy();

		if (200 != $trogdord->status) {
			die('Call to $game->destroy() should have been successful');
		}

		// Verify that we can't call \Trogdord\Game::createPlayer() after
		// destroying the object.
		try {
			$game->createPlayer("player 1");
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("createPlayer(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
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