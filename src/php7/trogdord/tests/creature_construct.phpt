--TEST--
Making sure \Trogdord\Creature::__construct() is private
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$creature = new \Trogdord\Creature();
?>
--EXPECTF--
%ACall to private Trogdord\Entity::__construct()%A