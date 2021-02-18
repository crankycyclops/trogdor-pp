--TEST--
Test Exception Classes
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
--FILE--
<?php

	///////////////////////////////////////////////////////////////////////////
	// Basically, I'm just going to try throwing each type of exception and  //
	// make sure there aren't any problems in the inheritance hierarchy that //
	// would cause me to get an error like, "class must implement Throwable" //
	// (I got bit by this once and don't want it to happen again.)           //
	///////////////////////////////////////////////////////////////////////////

	$code = 5;
	$message = "I'm a message!";

	// Maps exceptions to their parent in the hierarchy
	$exceptions = [
		'\Trogdord\Exception' => null,
		'\Trogdord\NetworkException' => '\Trogdord\Exception',
		'\Trogdord\RequestException' => '\Trogdord\Exception',
		'\Trogdord\FilterException'  => '\Trogdord\Exception',
		'\Trogdord\GameNotFound'     => '\Trogdord\RequestException',
		'\Trogdord\DumpSlotNotFound' => '\Trogdord\RequestException',
		'\Trogdord\EntityNotFound'   => '\Trogdord\RequestException',
		'\Trogdord\ResourceNotFound' => '\Trogdord\EntityNotFound',
		'\Trogdord\TangibleNotFound' => '\Trogdord\EntityNotFound',
		'\Trogdord\ThingNotFound'    => '\Trogdord\TangibleNotFound',
		'\Trogdord\ObjectNotFound'   => '\Trogdord\ThingNotFound',
		'\Trogdord\BeingNotFound'    => '\Trogdord\ThingNotFound',
		'\Trogdord\CreatureNotFound' => '\Trogdord\BeingNotFound',
		'\Trogdord\PlayerNotFound'   => '\Trogdord\BeingNotFound',
		'\Trogdord\PlaceNotFound'    => '\Trogdord\TangibleNotFound',
		'\Trogdord\RoomNotFound'     => '\Trogdord\PlaceNotFound',
	];

	foreach ($exceptions as $exceptionClass => $parent) {

		try {
			throw new $exceptionClass($message, $code);
		}

		catch (Exception $e) {

			if ($e->getMessage() != $message) {
				die(get_class($e) . ": mismatched message");
			}

			if ($code != $e->getCode()) {
				die(get_class($e) . ": mismatched code");
			}

			if (!is_null($parent) && !$e instanceof $parent) {
				die(get_class($e) . " should be an instance of $parent but is not");
			}
		}
	}

	echo "done!";

?>
--EXPECT--
done!
