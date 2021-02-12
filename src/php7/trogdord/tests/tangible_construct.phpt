--TEST--
Making sure \Trogdord\Tangible is abstract
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$tangible = new \Trogdord\Tangible();
?>
--EXPECTF--
%ACannot instantiate abstract class Trogdord\Tangible%A