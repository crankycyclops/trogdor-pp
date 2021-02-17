--TEST--
\Trogdord\Player::destroy()
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

		// Make sure the game is stopped. It should be by default, but
		// better safe than sorry.
		$game->stop();

		if (200 != $game->trogdord->status) {
			die('Stopping game should have been successful');
		}

		$player = $game->createPlayer("player 1");

		if (200 != $player->game->trogdord->status) {
			die('Creating game should have been successful');
		}

		try {
			$returnedPlayer = $game->getPlayer("player 1");
			if ($returnedPlayer->name != $player->name) {
				die('Player names don\'t match after creation.');
			}
		}

		catch (\Trogdord\PlayerNotFound $e) {
			die("'player 1' should exist after creating it.");
		}

		$player->destroy();

		if (200 != $player->game->trogdord->status) {
			die('Player destruction should have been successful');
		}

		try {
			$game->getPlayer("player 1");
			die('Player should no longer exist after being destroyed');
		}

		// Success: the player should no longer exist server-side
		catch (\Trogdord\PlayerNotFound $e) {}

		// Clean up
		$game->destroy();

		if (200 != $trogdord->status) {
			die('Call to $game->destroy() should have been successful');
		}

		// Verify that we can't call \Trogdord\Player::destroy() after
		// destroying the game object.
		try {
			$player->input("north");
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("\Trogdord\Player::destroy(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}

		// Verify that we can't call \Trogdord\Player::destroy() after
		// destroying the player object.
		$game = $trogdord->newGame('My Game', 'game.xml');
		$player = $game->createPlayer("player");

		$player->destroy();

		if (200 != $player->game->trogdord->status) {
			die('Call to $player->destroy() should have been successful');
		}

		try {
			$player->input("north");
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\EntityNotFound $e) {
			if ("Entity has already been destroyed" != $e->getMessage()) {
				die("\Trogdord\Player::destroy(): Call to method on invalidated player object resulted in incorrect message: '" . $e->getMessage() . "'. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}

		$game->destroy();
	}

	catch (Exception $e) {
		die("Caught instance of " . get_class($e) . ": " . $e->getMessage());
	}

	echo "done!";
?>
--EXPECT--
done!