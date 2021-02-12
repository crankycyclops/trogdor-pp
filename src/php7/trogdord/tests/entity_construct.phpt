--TEST--
Making sure \Trogdord\Entity is abstract
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$entity = new \Trogdord\Entity();
?>
--EXPECTF--
%ACannot instantiate abstract class Trogdord\Entity%A