--TEST--
\Trogdord\Game::start(), \Trogdord\Game::stop(), and \Trogdord\Game::isRunning()
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

        if (!$game instanceof \Trogdord\Game) {
            die('If call is successsful, \Trogdord::newGame() must return instance of \Trogdord\Game.');
        }

        if (!is_int($game->id)) {
            die('$game->id should be an integer.');
        }

        if (200 != $trogdord->status) {
            die('200 should be status after a successful request.');
        }

        // Game should start off in a stopped state by default
        if ($game->isRunning()) {
            die('Upon creation, game should default to a stopped state.');
        }

        $game->start();

        if (200 != $trogdord->status) {
            die('Call to start game should be successful.');
        }

        // Game should now be running
        if (!$game->isRunning()) {
            die('Game should have been started but seems to be stopped.');
        }

        $game->stop();

        if (200 != $trogdord->status) {
            die('Call to stop game should be successful.');
        }

        // Game should now be stopped
        if ($game->isRunning()) {
            die('Game should have been stopped but seems to be started.');
        }

        // Clean up
        $game->destroy();

        // Make sure invalidated game object methods can't be called
        try {
            $game->start();
            die('Call should not be successful after game has been destroyed and object has been invalidated.');
        } catch (\Trogdord\GameNotFound $e) {
            if ("Game has already been destroyed" != $e->getMessage()) {
                die("start(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
            }
        }

        try {
            $game->stop();
            die('Call should not be successful after game has been destroyed and object has been invalidated.');
        } catch (\Trogdord\GameNotFound $e) {
            if ("Game has already been destroyed" != $e->getMessage()) {
                die("stop(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
            }
        }

        try {
            $game->isRunning();
            die('Call should not be successful after game has been destroyed and object has been invalidated.');
        } catch (\Trogdord\GameNotFound $e) {
            if ("Game has already been destroyed" != $e->getMessage()) {
                die("isRunning(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
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