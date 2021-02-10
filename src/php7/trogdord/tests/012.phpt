--TEST--
\Trogdord\Game::entities() and \Trogdord\Game::getEntity()
--SKIPIF--
<?php if (!extension_loaded('trogdord')) die('skip The trogdord extension must be installed'); ?>
<?php require_once('inc/skipifconnectfailure.inc'); ?>
--FILE--
<?php

	require_once("inc/connect.inc");
	$trogdord = trogConnect();

	if (!$trogdord instanceof \Trogdord) {
		die($trogdord->getMessage());
	}

	try {

		// Requires sample game.xml to be installed under definitions path
		// configured in trogdord.ini.
		$game = $trogdord->newGame('My Game', 'game.xml');

		if (200 != $trogdord->status) {
			die('Creating game should have been successful');
		}

		///////////////////////////////////////////////////////////////////////////////
		// The remaining tests all rely on specific data to be present in the sample //
		// game.xml file. If that file changes, these tests might break or otherwise //
		// prove unreliable.                                                         //
		///////////////////////////////////////////////////////////////////////////////

		$types = [
			['entity', 'entities'],
			['tangible', 'tangibles'],
			['place', 'places'],
			['thing', 'things'],
			['being', 'beings'],
			['resource', 'resources'],
			['room', 'rooms'],
			['object', 'objects'],
			['player', 'players'],
			['creature', 'creatures']
		];

		foreach ($types as $type) {

			$entities = call_user_func_array([$game, $type[1]], []);

			if (!is_array($entities)) {
				die("$type: return value should be an array");
			}

			foreach ($entities as $entity) {

				if (!is_array($entity)) {
					die("{$type[1]}: each entity returned by \$game->entities() should be an array");
				}

				if (!isset($entity['name'])) {
					die("{$type[1]}: \$entity[\"name\"] should be defined but isn't");
				}

				if (!isset($entity['type'])) {
					die("{$type[1]}: \$entity[\"type\"] should be defined but isn't");
				}

				if (!is_string($entity['name'])) {
					die("{$type[1]}: \$entity[\"name\"] should be a string but isn't");
				}

				if (!is_string($entity['type'])) {
					die("{$type[1]}: \$entity[\"type\"] should be a string but isn't");
				}

				if ("entity" != $type[0] && $type[0] != $entity['type']) {
					die("{$type[1]}: expected \$entity[\"type\"] for '{$entity['name']}' to be '{$type[0]}' but got '{$entity['type']}'");
				}
			}

			// TODO: test getEntity() and friends
		}

		// Clean up
		$game->destroy();

		if (200 != $trogdord->status) {
			die('Game destruction should have been successful');
		}

		// Make sure invalidated game object's methods can't be called again
		try {
			$game->entities();
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("entities(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}

		try {
			$game->getEntity("candle");
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("getEntity(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}

		// TODO: do the same for other entity types
	}

	catch (Exception $e) {
		die("Caught instance of " . get_class($e) . ": " . $e->getMessage());
	}

	echo "done!";
?>
--EXPECT--
done!