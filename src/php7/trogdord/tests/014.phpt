--TEST--
\Trogdord\Game::statistics()
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

		$statistics = $game->statistics();

		if (200 != $trogdord->status) {
			die('Call to $game->statistics() should have been successful');
		}

		if (!is_array($statistics)) {
			die('$game->statistics() should return an array.');
		}

		// Checking for unknown keys will force me to keep the tests up to
		// date when I add new data later.
		$validKeys = ['is_running', 'created', 'players', 'current_time'];

		foreach ($statistics as $key => $value) {
			if (!in_array($key, $validKeys)) {
				die("Unknown key '$key' in game statistics. This probably means the test should be updated ;)");
			}
		}

		if (!isset($statistics['is_running'])) {
			die('$statistics["is_running"] should be set but is not.');
		}

		if (!is_bool($statistics['is_running'])) {
			die('$statistics["is_running"] should be a boolean but isn\'t.');
		}

		if (!isset($statistics['created'])) {
			die('$statistics["created"] should be set but is not.');
		}

		if (!is_string($statistics['created'])) {
			die('$statistics["created"] should be a string but isn\'t.');
		}

		if (!isset($statistics['players'])) {
			die('$statistics["players"] should be set but is not.');
		}

		if (!is_int($statistics['players'])) {
			die('$statistics["players"] should be an int but isn\'t.');
		}

		if (!isset($statistics['current_time'])) {
			die('$statistics["current_time"] should be set but is not.');
		}

		if (!is_int($statistics['current_time'])) {
			die('$statistics["current_time"] should be an int but isn\'t.');
		}

		// Clean up
		$game->destroy();

		if (200 != $trogdord->status) {
			die('Call to $game->destroy() should have been successful');
		}

		// Verify that we can't call \Trogdord\Game::statistics() after
		// destroying the object.
		try {
			$game->statistics();
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("statistics(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
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