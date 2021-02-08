--TEST--
\Trogdord::dump() and \Trogdord::restore()
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
<?php require_once('inc/skipifconnectfailure.inc'); ?>
--FILE--
<?php

    /**************************************************************************\
     * IMPORTANT: this unit tests should be run twice, once with state turned *
     * on in trogdord.ini and once with state turned off.                     *
    \**************************************************************************/

	require_once("inc/connect.inc");
	$trogdord = trogConnect();

	if (!$trogdord instanceof \Trogdord) {
		die($trogdord->getMessage());
	}

	try {

		$config = $trogdord->config();

        // If state is enabled, \Trogdord::dump() should succeed
        if ($config['state.enabled']) {

            // This requires the sample game.xml file that comes bundled with
            // trogdord to be installed under the configured definitions path.
            $game1 = $trogdord->newGame('My Game 1', 'game.xml');

            if (200 != $trogdord->status) {
                die('Failed to create My Game. Is game.xml installed under the configured defintiions path?');
            }

            $game2 = $trogdord->newGame('My Game 2', 'game.xml');

            if (200 != $trogdord->status) {
                die('Failed to create My Game. Is game.xml installed under the configured defintiions path?');
            }

            $trogdord->dump();

            if (200 != $trogdord->status) {
                die('When state is enabled, \Trogdord::dump() should succeed and \Trogdord::$status should be set to 200.');
            }

            $gameId1 = $game1->id;
            $gameId2 = $game2->id;

            // Destroy the games without destroying their dumps
            $game1->destroy(false);
            $game2->destroy(false);

            // Verify that the games were destroyed
            try {
                $trogdord->getGame($gameId1);
                die('My Game 1 should have been destroyed');
            }

            catch (\Trogdord\GameNotFound $e) {}

            try {
                $trogdord->getGame($gameId2);
                die('My Game 2 should have been destroyed');
            }

            catch (\Trogdord\GameNotFound $e) {}

            $trogdord->restore();

            // Verify that the games exist again
            try {
                $trogdord->getGame($gameId1);
            }

            catch (\Trogdord\GameNotFound $e) {
                die('My Game 1 should exist after call to \Trogdord::restore()');
            }

            try {
                $trogdord->getGame($gameId2);
            }

            catch (\Trogdord\GameNotFound $e) {
                die('My Game 2 should exist after call to \Trogdord::restore()');
            }
        }

        // If state is disabled, we should get a 501 unsupported status
        else {

            try {
                $trogdord->dump();
            }

            catch (\Trogdord\RequestException $e) {

                if (501 != $e->getCode()) {
                    die('\Trogdord::dump() should result in a 501 unsupported status when state is disabled.');
                }

                else if (501 != $trogdord->status) {
                    die('\Trogdord::$status was not set to 501 unsupported status when state was disabled.');
                }
            }

            try {
                $trogdord->restore();
            }

            catch (\Trogdord\RequestException $e) {

                if (501 != $e->getCode()) {
                    die('\Trogdord::restore() should result in a 501 unsupported status when state is disabled.');
                }

                else if (501 != $trogdord->status) {
                    die('\Trogdord::$status was not set to 501 unsupported status when state was disabled.');
                }
            }
        }

		echo "done!";
	}

	catch (Exception $e) {
		die($e->getMessage());
	}
?>
--EXPECT--
done!