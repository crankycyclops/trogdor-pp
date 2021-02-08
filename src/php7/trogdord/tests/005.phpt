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
var_dump($games);
exit;
		if (!is_array($games)) {
			die('\Trogdord::games() should return an array.');
		} else if (count($games) > 0) {
			die('\Trogdord::games() should return an empty array if no games have been created.');
		} else if (200 != $trogdord->status) {
			die('\Trogdord::$status should be 200 after a successful request.');
		}

		// Create games matching various criteria.
		$game1 = $trogdord->newGame('aaa', 'game.xml');
		$game2 = $trogdord->newGame('aaa', 'game.xml');
		$game3 = $trogdord->newGame('bbb', 'game.xml');
		$game4 = $trogdord->newGame('bbb', 'game.xml');

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

		// TODO: test with both filters and meta keys
		echo "done!";
	}

	catch (Exception $e) {
		die($e->getMessage());
	}
?>
--EXPECT--
done!