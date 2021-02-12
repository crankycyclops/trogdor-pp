--TEST--
Making sure \Trogdord\Resource::__construct() is private
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	$resource = new \Trogdord\Resource();
?>
--EXPECTF--
%ACall to private Trogdord\Entity::__construct() from invalid context%A