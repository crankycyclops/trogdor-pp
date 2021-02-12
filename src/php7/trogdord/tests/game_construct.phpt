--TEST--
Making sure \Trogdord\Game::__construct() is private
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	// Make sure that we can't call \Trogdord\Game's constructor (\Trogdord
	// should produce instances with the getGame and newGame methods.)
	$game = new \Trogdord\Game();
?>
--EXPECTF--
%ACall to private Trogdord\Game::__construct()%A