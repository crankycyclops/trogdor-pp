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

		//////////////////

		// Test \Trogdord::newGame() with optional meta
		$gameWithMeta = $trogdord->newGame('My Game', 'game.xml', ['str' => 'value1', 'bool' => true, 'int' => 3]);

		if (!$gameWithMeta instanceof \Trogdord\Game) {
			die('If call is successsful, \Trogdord::newGame() must return instance of \Trogdord\Game.');
		}

		$meta = $gameWithMeta->getMeta();

		if (!isset($meta['str'])) {
			die("\$meta['str'] should be set since it was part of the call to \Trogdord::newGame()");
		}

		else if (!isset($meta['int'])) {
			die("\$meta['int'] should be set since it was part of the call to \Trogdord::newGame()");
		}

		else if (!isset($meta['bool'])) {
			die("\$meta['bool'] should be set since it was part of the call to \Trogdord::newGame()");
		}

		// Note that meta values are always strings. This just shows the values were
		// converted properly.
		if ('value1' != $meta['str']) {
			die("\$meta['str'] value is incorrect");
		}

		else if ('1' != $meta['bool']) {
			die("\$meta['bool'] value is incorrect");
		}

		else if ('3' != $meta['int']) {
			die("\$meta['int'] value is incorrect");
		}

		echo "done!";
	}

	catch (Exception $e) {
		die($e->getMessage());
	}
?>
--EXPECT--
done!