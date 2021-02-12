--TEST--
Making sure \Trogdord\Object::__construct() is private
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$object = new \Trogdord\Object();
?>
--EXPECTF--
%ACall to private Trogdord\Entity::__construct() from invalid context%A