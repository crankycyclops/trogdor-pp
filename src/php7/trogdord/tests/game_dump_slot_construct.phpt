--TEST--
Making sure \Trogdord\Game\Dump\Slot::__construct() is private
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$slot = new \Trogdord\Game\Dump\Slot();
?>
--EXPECTF--
%ACall to private Trogdord\Game\Dump\Slot::__construct()%A