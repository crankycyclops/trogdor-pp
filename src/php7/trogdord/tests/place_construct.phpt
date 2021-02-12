--TEST--
Making sure \Trogdord\Place is abstract
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$place = new \Trogdord\Place();
?>
--EXPECTF--
%ACannot instantiate abstract class Trogdord\Place%A