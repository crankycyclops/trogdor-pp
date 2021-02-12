--TEST--
Making sure \Trogdord\Thing is abstract
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$thing = new \Trogdord\Thing();
?>
--EXPECTF--
%ACannot instantiate abstract class Trogdord\Thing%A