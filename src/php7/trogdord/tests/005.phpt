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

		// TODO: test with both filters and meta keys

		echo "done!";
	}

	catch (Exception $e) {
		die($e->getMessage());
	}
?>
--EXPECT--
done!