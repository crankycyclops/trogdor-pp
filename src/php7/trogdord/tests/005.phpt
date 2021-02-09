--TEST--
\Trogdord::games()
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

		// Test with no games running
		$games = $trogdord->games();

		if (!is_array($games)) {
			die('\Trogdord::games() should return an array.');
		} else if (count($games) > 0) {
			die('\Trogdord::games() should return an empty array if no games have been created.');
		} else if (200 != $trogdord->status) {
			die('\Trogdord::$status should be 200 after a successful request.');
		}

		// Create games matching various criteria.
		$game1 = $trogdord->newGame('aaa', 'game.xml', ['key' => 'value']);
		$game2 = $trogdord->newGame('aaa', 'game.xml', ['key' => 'value']);
		$game3 = $trogdord->newGame('bbb', 'game.xml', ['key' => 'value']);
		$game4 = $trogdord->newGame('bbb', 'game.xml', ['key' => 'value']);

		$game1->stop();
		$game3->stop();

		$game2->start();
		$game4->start();

		// Get list with no filters
		$games = $trogdord->games();

		if (!is_array($games)) {
			die('\Trogdord::games() should return an array.');
		} else if (4 != count($games)) {
			die('\Trogdord::games() should return exactly 4 games.');
		} else if (200 != $trogdord->status) {
			die('\Trogdord::$status should be 200 after a successful request.');
		}

		////////

		$stopped = $trogdord->games(['is_running' => false]);

		if (2 != count($stopped)) {
			die('There are only two stopped games.');
		}

		foreach ($stopped as $game) {
			if ($game['id'] != $game1->id && $game['id'] != $game3->id) {
				die('Got a started game when filtering for stopped games.');
			}
		}

		////////

		$started = $trogdord->games(['is_running' => true]);

		if (2 != count($started)) {
			die('There are only two started games.');
		}

		foreach ($started as $game) {
			if ($game['id'] != $game2->id && $game['id'] != $game4->id) {
				die('Got a stopped game when filtering for started games.');
			}
		}

		////////

		$mixed = $trogdord->games(['is_running' => true, 'name_starts' => 'a']);

		if (1 != count($mixed)) {
			die("There is only one started game that starts with 'a'");
		}

		if ($mixed[0]['id'] != $game2->id) {
			die("Got the wrong game back when filtering for games that are started and whose names start with 'a'");
		}

		////////

		$union = $trogdord->games([['is_running' => true], ['name_starts' => 'b']]);

		if (3 != count($union)) {
			die("There are 3 games that are started or that start with 'b'");
		}

		foreach ($started as $game) {
			if ($game['id'] != $game2->id && $game['id'] != $game3->id && $game['id'] != $game4->id) {
				die("Got a game that doesn't match the filter union.");
			}
		}

		////////

		$gamesWithMeta = $trogdord->games([], ['key']);

		foreach ($gamesWithMeta as $game) {
			if (!isset($game['key'])) {
				die('Meta key not returned when included as second optional argument to \Trogdord::games()');
			}
		}

		////////

		// Clean up
		$game1->destroy();
		$game2->destroy();
		$game3->destroy();
		$game4->destroy();

		echo "done!";
	}

	catch (Exception $e) {
		die($e->getMessage());
	}
?>
--EXPECT--
done!