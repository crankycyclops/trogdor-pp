--TEST--
\Trogdord::newGame() and getGame()
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

		// 1. Attempt to get a game that doesn't exist
		try {
			$trogdord->getGame(10000);
			die("\Trogdord::getGame() should throw instance of \Trogdord\GameNotFound when game id doesn't exist.");
		}

		catch (\Trogdord\GameNotFound $e) {}

		// Requires sample game.xml to be installed under definitions path
		// configured in trogdord.ini.
		$game = $trogdord->newGame('My Game', 'game.xml');

		if (!$game instanceof \Trogdord\Game) {
			die('If call is successsful, \Trogdord::newGame() must return instance of \Trogdord\Game.');
		}

		if (!is_int($game->id)) {
			die('$game->id should be an integer.');
		}

		$game2 = $trogdord->getGame($game->id);

		if ($game2->id != $game->id) {
			die("$trogdord->getGame($game->id) should return object with equivalent id.");
		}

		// TODO: test newGame() with optional meta
		echo "done!";
	}

	catch (Exception $e) {
		die($e->getMessage());
	}
?>
--EXPECT--
done!