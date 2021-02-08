--TEST--
\Trogdord::definitions()
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

		$definitions = $trogdord->definitions();

        if (!is_array($definitions)) {
            die('$definitions must be an array.');
        }

        foreach ($definitions as $definition) {
            if (!is_string($definition)) {
                die('All entries in $definitions must be strings representing definition file names.');
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