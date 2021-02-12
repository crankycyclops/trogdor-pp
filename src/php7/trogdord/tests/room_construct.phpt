--TEST--
Making sure \Trogdord\Room::__construct() is private
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$room = new \Trogdord\Room();
?>
--EXPECTF--
%ACall to private Trogdord\Entity::__construct()%A