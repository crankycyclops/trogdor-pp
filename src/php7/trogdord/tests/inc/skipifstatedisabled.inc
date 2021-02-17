<?php

	require_once('connect.inc');

    $trogdord = trogConnect();

    if (!$trogdord instanceof \Trogdord) {
        die("skip Failed to connect - " . $trogdord->getMessage());
    }

    $config = $trogdord->config();

    if (!is_array($config)) {
        die("skip \$config should be an array but isn't");
    }

    if (!$config['state.enabled']) {
        die('skip state is disabled');
    }

?>
