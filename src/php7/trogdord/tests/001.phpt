--TEST--
\Trogdord::__construct()
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
<?php require_once('inc/skipifconnectfailure.inc'); ?>
--FILE--
<?php

	require_once("inc/connect.inc");
	$trogdord = trogConnect();

	if (!$trogdord instanceof \Trogdord) {
		echo $trogdord->getMessage();
	} else {
		echo "done!";
	}
?>
--EXPECT--
done!
