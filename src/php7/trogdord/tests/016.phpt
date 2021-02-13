--TEST--
\Trogdord\Entity::output()
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

		$player = $game->createPlayer("player 1");

		if (200 != $player->game->trogdord->status) {
			die('Creating game should have been successful');
		}

		////////////////////////////////////////////
		// Part 1: Test output on default channel //
		////////////////////////////////////////////

		// Verify that we receive output on the default channel. This also has
		// the effect of flushing the output buffer so that we can check for
		// the output message that comes in the next part of the test.
		$output = $player->output();

		if (200 != $player->game->trogdord->status) {
			die('Getting output should have been successful');
		}

		if (!is_array($output)) {
			die('Output returned by $player->output() should be an array.');
		}

		if (!isset($output[0]['timestamp'])) {
			die('Timestamp not set on output message.');
		}

		if (!is_int($output[0]['timestamp'])) {
			die('Timestamp should be an int but is not.');
		}

		if (!isset($output[0]['content'])) {
			die('Content not set on output message.');
		}

		if (!is_string($output[0]['content'])) {
			die('Content should be a string but is not.');
		}

		if (0 != count($player->output())) {
			die('Output buffer should have been flushed on default channel but was not.');
		}

		// Send output to the default channel
		$player->output(null, "I'm a message");

		if (200 != $player->game->trogdord->status) {
			die('Setting message on default channel should have been successful');
		}

		$output = $player->output();

		if (200 != $player->game->trogdord->status) {
			die('Getting output after setting it should have been successful.');
		}

		if (1 != count($output)) {
			die('Output on default channel should be exactly 1 message after sending a message to the buffer.');
		}

		// I'm trimming the result because the decision to add extra whitespace like
		// newline characters is for the server to make, not the PHP extension.
		if ("I'm a message" != trim($output[0]['content'])) {
			die("Output message on default channel should send what was sent in previous call.");
		}

		/////////////////////////////////////////////
		// Part 2: Test output on specific channel //
		/////////////////////////////////////////////

		// The test channel should have received no messages on the "test"
		// channel before the following call to send one. If it did, that's
		// something that should be fixed.
		$output = $player->output("test");

		if (200 != $player->game->trogdord->status) {
			die('Getting output should have been successful');
		}

		if (!is_array($output)) {
			die('Output returned by $player->output() should be an array.');
		}

		if (0 != count($output)) {
			die('Output on test channel should have received no messages.');
		}

		// Send output to the "test" channel
		$player->output("test", "I'm a message");

		if (200 != $player->game->trogdord->status) {
			die('Setting message on channel "test" should have been successful');
		}

		$output = $player->output("test");

		if (200 != $player->game->trogdord->status) {
			die('Getting output after setting it should have been successful.');
		}

		if (1 != count($output)) {
			die('Output on channel "test" should be exactly 1 message after sending a message to the buffer.');
		}

		if ("I'm a message" != trim($output[0]['content'])) {
			die('Output message on channel "test" should send what was sent in previous call.');
		}

		// Clean up
		$game->destroy();

		if (200 != $trogdord->status) {
			die('Call to $game->destroy() should have been successful');
		}

        // TODO: also test calling after destroying entity

		// Verify that we can't call \Trogdord\Entity::output() after
		// destroying the object.
		try {
			$player->output();
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("\Trogdord\Entity::output(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
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