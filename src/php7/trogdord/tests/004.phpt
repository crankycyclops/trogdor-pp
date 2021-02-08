--TEST--
\Trogdord::dump() and \Trogdord::restore()
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

		$config = $trogdord->config();

        // If state is enabled, \Trogdord::dump() should succeed
        if ($config['state.enabled']) {

            // TODO: create a game and verify that it actually gets dumped
            // TODO: destroy the game without destroying the dump, then verify that restore restores it

            $trogdord->dump();

            if (200 != $trogdord->status) {
                die('When state is enabled, \Trogdord::dump() should succeed and \Trogdord::$status should be set to 200.');
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
        }

		echo "done!";
	}

	catch (Exception $e) {
		die($e->getMessage());
	}
?>
--EXPECT--
done!