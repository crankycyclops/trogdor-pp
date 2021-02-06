--TEST--
\Trogdord::statistics()
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

		// I only check these so that if I forget to write checks for additional stats
		// that are added later, I'll get a nice obvious warning in the form of a failed test :)
		$validKeys = [
			'lib_version',
			'version',
			'players'
		];

		$stats = $trogdord->statistics();

		if (!is_array($stats)) {
			die("\$trogdord->statistics() did not return an array.");
		}

		foreach ($stats as $key => $value) {
			if (!in_array($key, $validKeys, true)) {
				die("Unexpected key '$key' encountered in response.");
			}
		}

		if (!isset($stats['lib_version'])) {
			die("Expected \$stats['lib_version'] to be set.");
		}

		if (!is_array($stats['lib_version'])) {
			die("Expected \$stats['lib_version'] to be an array.");
		}

		foreach ($stats['lib_version'] as $key => $value) {
			if (!is_int($value)) {
				die("Expected \$stats['lib_version'][$value] to be an integer.");
			}
		}

		if (!isset($stats['version'])) {
			die("Expected \$stats['version'] to be set.");
		}

		if (!is_array($stats['version'])) {
			die("Expected \$stats['version'] to be an array.");
		}

		foreach ($stats['version'] as $key => $value) {
			if (!is_int($value)) {
				die("Expected \$stats['version'][$value] to be an integer.");
			}
		}

		if (!isset($stats['players'])) {
			die("Expected \$stats['players'] to be set.");
		}

		if (!is_int($stats['players'])) {
			die("Expected \$stats['players'] to be an integer.");
		}

		if ($trogdord->status != 200) {
			die("Expected \$trogdord->status to be 200, but got {$trogdord->status}.");
		}

		echo "done!";
	}

	catch (Exception $e) {
		die($e->getMessage());
	}
?>
--EXPECT--
done!
