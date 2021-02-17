--TEST--
Making sure \Trogdord\Game\Dump::__construct() is private
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$dump = new \Trogdord\Game\Dump();
?>
--EXPECTF--
%ACall to private Trogdord\Game\Dump::__construct()%A