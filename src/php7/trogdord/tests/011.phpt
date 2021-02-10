--TEST--
\Trogdord\Game::getMeta() and \Trogdord\Game::setMeta()
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

        $game->setMeta(['str' => 'string', 'bool' => false, 'int' => 5]);

		if (200 != $trogdord->status) {
			die('Setting meta should have been successful');
		}

        $meta = $game->getMeta(['str', 'bool', 'int', 'empty']);

		if (200 != $trogdord->status) {
			die('Getting meta should have been successful');
		}

        foreach (['str', 'int', 'empty'] as $key) {
            if (!isset($meta[$key])) {
                die("Meta value '$key' should be set but isn't.");
            }
        }

        if ("string" != $meta['str']) {
            die("Meta value 'str' is incorrect: expected 'string' but got '{$meta['str']}");
        }

        if ("5" != $meta['int']) {
            die("Meta value 'int' is incorrect: expected '5' but got '{$meta['int']}");
        }

        if ("0" != $meta['bool']) {
            die("Meta value 'bool' is incorrect: expected '0' but got '{$meta['bool']}'");
        }

        // Clean up
        $game->destroy();

		if (200 != $trogdord->status) {
			die('Game destruction should have been successful');
		}

		// Make sure invalidated game object's methods can't be called again
		try {
			$game->getMeta(["title"]);
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("destroy(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}

		try {
			$game->setMeta(["title" => "wee!"]);
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("destroy(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
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