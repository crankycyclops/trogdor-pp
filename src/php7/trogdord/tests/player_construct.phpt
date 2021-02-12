--TEST--
Making sure \Trogdord\Player::__construct() is private
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$player = new \Trogdord\Player();
?>
--EXPECTF--
%ACall to private Trogdord\Entity::__construct()%A