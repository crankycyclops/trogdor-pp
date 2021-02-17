--TEST--
\Trogdord\Game\Dump::slots()
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

		// TODO: I think this is where the exception is being thrown
		$slots = $dump->slots();

		if (200 != $dump->trogdord->status) {
			die('Getting list of dump slots should have been successful');
		}

		if (!is_array($slots)) {
			die('$slots should be an array but isn\'t');
		}

		if (1 != count($slots)) {
			die('$slots should contain exactly 1 value');
		}

		if (!isset($slots[0]['slot'])) {
			die('$slots[0][\'slot\'] should be set but is not');
		}

		if (!is_int($slots[0]['slot'])) {
			die('$slots[0][\'slot\'] should be an integer but is not');
		}

		if (!isset($slots[0]['timestamp_ms'])) {
			die('$slots[0][\'timestamp_ms\'] should be set but is not');
		}

		if (!is_int($slots[0]['timestamp_ms']) && !is_double($slots[0]['timestamp_ms'])) {
			die('$slots[0][\'timestamp\'] should be an integer or double but is not');
		}

		// Destroy the dump and confirm that it's properly invalidated
		$dump->destroy();

		if (200 != $trogdord->status) {
			die('Dump destruction should have been successful but was not');
		}

		try {
			$dump->slots();
		} catch (\Trogdord\GameNotFound $e) {
			if ("Dump has already been destroyed" != $e->getMessage()) {
				die("\Trogdord\Game\Dump::slots(): Call to method on invalidated dump object resulted in incorrect message: '" . $e->getMessage() . "'. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
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