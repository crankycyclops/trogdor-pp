<?php

	$skipOnConnectFailure = getenv("TROGDORD_TEST_SKIP_CONNECT_FAILURE") ? getenv("TROGDORD_TEST_SKIP_CONNECT_FAILURE") : true;

	$host = getenv("TROGDORD_TEST_HOST") ? getenv("TROGDORD_TEST_HOST") : "localhost";
	$port = getenv("TROGDORD_TEST_PORT") ? getenv("TROGDORD_TEST_PORT") : 1040;

	function trogConnect() {

		global $host, $port;

		try {
			return new \Trogdord($host, $port);
		} catch (Exception $e) {
			return $e;
		}
	}
?>
