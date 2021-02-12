--TEST--
Making sure \Trogdord\Being is abstract
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$being = new \Trogdord\Being();
?>
--EXPECTF--
%ACannot instantiate abstract class Trogdord\Being%A