<?php

	require_once('connect.inc');

	if ($skipOnConnectFailure) {

		$trogdord = trogConnect();

		if (!$trogdord instanceof \Trogdord) {
			die("skip Failed to connect - " . $trogdord->getMessage());
		}
	}

?>
