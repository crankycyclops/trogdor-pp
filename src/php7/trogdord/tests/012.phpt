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

		// We need at least one player to test all entity types
		$player = $game->createPlayer("player");

		if (200 != $trogdord->status) {
			die('Creating player in game should have been successful');
		}

		if (!$player instanceof \Trogdord\Player) {
			die('$player should be an instance of \Trogdord\Player');
		}

		///////////////////////////////////////////////////////////////////////////////
		// The remaining tests all rely on specific data to be present in the sample //
		// game.xml file. If that file changes, these tests might break or otherwise //
		// prove unreliable.                                                         //
		///////////////////////////////////////////////////////////////////////////////

		$types = [
			['entity',   'entities'],
			['tangible', 'tangibles'],
			['place',    'places'],
			['thing',    'things'],
			['being',    'beings'],
			['resource', 'resources'],
			['room',     'rooms'],
			['object',   'objects'],
			['player',   'players'],
			['creature', 'creatures']
		];

		$hierarchy = [
			'entity'   => ['entity', 'tangible', 'place', 'thing', 'being', 'resource', 'room', 'object', 'player', 'creature'],
			'tangible' => ['tangible', 'place', 'thing', 'being', 'room', 'object', 'player', 'creature'],
			'place'    => ['place', 'room'],
			'thing'    => ['thing', 'being', 'room', 'object', 'player', 'creature'],
			'being'    => ['being', 'player', 'creature'],
			'resource' => ['resource'],
			'room'     => ['room'],
			'object'   => ['object'],
			'player'   => ['player'],
			'creature' => ['creature']
		];

		// We'll index the entities we encounter here so that we can test them
		// later with \Trogdord\Game::getEntity() and friends
		$entities = [
			'entity'   => [],
			'tangible' => [],
			'place'    => [],
			'thing'    => [],
			'being'    => [],
			'resource' => [],
			'room'     => [],
			'object'   => [],
			'player'   => [],
			'creature' => []
		];

		// Test \Trogdord\Game::entities() and friends
		foreach ($types as $type) {

			$list = call_user_func_array([$game, $type[1]], []);

			if (!is_array($list)) {
				die("{$type[1]}: return value should be an array");
			}

			foreach ($list as $entity) {

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

				if (!in_array($entity['type'], $hierarchy[$type[0]])) {
					echo("{$type[1]}: expected \$entity[\"type\"] for '{$entity['name']}' to inherit from '{$type[0]}', but '{$entity['type']}' doesn't\n");
				}

				// This will be used for our test of \Trogdord\Game::getEntity() and friends
				$entities[$type[0]][] = $entity['name'];
			}
		}

		// Test \Trogdord\Game::getEntity() and friends
		foreach ($types as $type) {

			foreach ($entities as $eType => $group) {

				if (0 == count($group)) {
					die("$eType: no entities of this type were listed, so we can't test its corresponding getter :(");
				}
			}

			// TODO: finish
		}

		// Clean up
		$game->destroy();

		if (200 != $trogdord->status) {
			die('Game destruction should have been successful');
		}

		// TODO: do foreach $types on this so we test each one
		// Make sure invalidated game object's methods can't be called again
		try {
			$game->entities();
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("entities(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}

		// TODO: do foreach $types on this so we test each one
		try {
			$game->getEntity($entities['entity'][0]);
			die('Call should not be successful after game has been destroyed and object has been invalidated.');
		} catch (\Trogdord\GameNotFound $e) {
			if ("Game has already been destroyed" != $e->getMessage()) {
				die("getEntity(): Call to method on invalidated game object resulted in incorrect message. This could indicate that a request was made to the server, in which case the PHP method needs to be fixed.");
			}
		}
	}

	catch (Exception $e) {
		die("Caught instance of " . get_class($e) . ": " . $e->getMessage());
	}

	echo "done!";
?>
--EXPECT--
done!