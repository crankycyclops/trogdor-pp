'use strict';

const Trogdord = require('./trogdord');

/**
 * Class representing a game that exists inside an instance of trogdord.
 */
class Game {

	// This type should only be internally instantiable
	#Dump = require('./dump/dump');

	// Private class definitions for each entity type
	#EntityTypes = {
		resource: require('./resource'),
		room: require('./room'),
		object: require('./object'),
		creature: require('./creature'),
		player: require('./player')
	};

	// The game's numeric id
	#id;

	// The game's name
	#name;

	// The game's definition file
	#definition;

	// UNIX timestamp when the game was created
	#created;

	// Instance of Trogdord that spawned this instance of Game
	#trogdord;

	/**
	 * Returns a promise that resolves to an array of all entities in the game.
	 *
	 * @param {String} scope The type of entities to return
	 * @param {Boolean} returnEntities If true, return an array of Entities instead of simple objects
	 */
	#getEntityList = (scope, returnEntities) => {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: scope,
				action: "list",
				args: {game_id: this.#id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				if (returnEntities) {

					let entities = [];

					response.entities.forEach((entity, index) => {
						entities.push(new this.#EntityTypes[entity.type](this, entity.name));
					});

					resolve(entities);
				}

				else {
					resolve(response.entities);
				}

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to an object representing an entity in a game.
	 *
	 * @param {String} scope The desired entity's type
	 * @param {String} name The desired entity's name
	 */
	#getEntityObject = (scope, name) => {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: scope,
				args: {
					game_id: this.#id,
					name: name
				}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(new this.#EntityTypes[response.entity.type](
					this, response.entity.name)
				);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Create a new object representing a game inside an instance of trogdord.
	 *
	 * @param {Integer} id The game's id
	 * @param {String} name The game's name
	 * @param {String} definition The game's definition filename
	 * @param {Integer} created UNIX timestamp when the game was created
	 * @param {Object} trogdord Connection to the instance of trogdord the game belongs to
	 */
	constructor(id, name, definition, created, trogdord) {

		this.#id = id;
		this.#name = name;
		this.#definition = definition;
		this.#created = created;
		this.#trogdord = trogdord;
	}

	/**
	 * Returns the game's id.
	 */
	get id() {

		return this.#id;
	}

	/**
	 * Returns the game's name.
	 */
	get name() {

		return this.#name;
	}

	/**
	 * Returns the game's definition.
	 */
	get definition() {

		return this.#definition;
	}

	/**
	 * Returns the UNIX timestamp when the game was created.
	 */
	get created() {

		return this.#created;
	}

	/**
	 * Returns the connection to trogdord associated with the game.
	 */
	get trogdord() {

		return this.#trogdord;
	}

	/**
	 * Returns a promise that resolves to an object containing game-specific
	 * statistics.
	 */
	statistics() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: "game",
				action: "statistics",
				args: {id: this.#id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				delete response.status;
				resolve(response);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to true if the game is running and false
	 * if it's not.
	 */
	isRunning() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: "game",
				action: "is_running",
				args: {id: this.#id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response.is_running);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to the current time in the game.
	 */
	getTime() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: "game",
				action: "time",
				args: {id: this.#id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response.current_time);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Starts the game.
	 */
	start() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "set",
				scope: "game",
				action: "start",
				args: {id: this.#id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Stops the game.
	 */
	stop() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "set",
				scope: "game",
				action: "stop",
				args: {id: this.#id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Destroys the game. From this point forward, the Game object will be
	 * invalid and network requests that originate from it will result in a
	 * 404 game not found error.
	 * 
	 * If provided, the optional parameter destroyDump will determine whether
	 * or not to also destroy all dumps of the game (if any.)
	 * 
	 * @param {Boolean} destroyDump Whether or not to destroy all dumps of the game as well (if any)
	 */
	destroy(destroyDump = true) {

		return new Promise((resolve, reject) => {

			if ('boolean' != typeof destroyDump) {
				throw new Error('optional argument destroyDump must be a boolean value');
			}

			this.#trogdord.makeRequest({
				method: "delete",
				scope: "game",
				args: {id: this.#id, delete_dump: destroyDump}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Dumps the game and returns an instance of Dump.
	 */
	dump() {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "post",
				scope: "game",
				action: "dump",
				args: {id: this.#id}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(new this.#Dump(
					this.id,
					this.name,
					this.definition,
					this.created,
					this.trogdord
				));

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to game-specific metadata. If the keys
	 * argument is specified, it should contain the meta keys whose values
	 * should be returned. If no argument is given, all existing metadata will
	 * be returned.
	 *
	 * @param {Array} keys Metadata values to retrieve (optional)
	 */
	getMeta(keys = []) {

		let args = {id: this.#id};

		// If the user just wants to retrieve a single metadata value, let
		// them pass in just that string as a shortcut.
		if ('string' == typeof keys) {
			keys = [keys];
		}

		if (keys.length) {
			args.meta = keys;
		}

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "get",
				scope: "game",
				action: "meta",
				args: args
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response.meta);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Takes as input an object of key, value pairs and sets them as metadata
	 * for the game. Returns a promise that notifies the client of whether or
	 * not the trogdord request was successful.
	 *
	 * @param {Object} meta Metadata key, value pairs to set on the game
	 */
	setMeta(meta) {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "set",
				scope: "game",
				action: "meta",
				args: {
					id: this.#id,
					meta: meta
				}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(response);

			}).catch(error => {
				reject(error);
			});
		});
	}

	/**
	 * Returns a promise that resolves to an array of all entities in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Entities instead of simple objects (optional)
	 */
	entities(returnEntities = true) {

		return this.#getEntityList('entity', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an array of all tangibles in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Tangibles instead of simple objects (optional)
	 */
	tangibles(returnEntities = true) {

		return this.#getEntityList('tangible', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an array of all places in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Places instead of simple objects (optional)
	 */
	places(returnEntities = true) {

		return this.#getEntityList('place', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an array of all things in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Things instead of simple objects (optional)
	 */
	things(returnEntities = true) {

		return this.#getEntityList('thing', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an array of all beings in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Beings instead of simple objects (optional)
	 */
	beings(returnEntities = true) {

		return this.#getEntityList('being', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an array of all resources in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Resources instead of simple objects (optional)
	 */
	resources(returnEntities = true) {

		return this.#getEntityList('resource', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an array of all rooms in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Rooms instead of simple objects (optional)
	 */
	rooms(returnEntities = true) {

		return this.#getEntityList('room', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an array of all objects in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Objects instead of simple objects (optional)
	 */
	objects(returnEntities = true) {

		return this.#getEntityList('object', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an array of all creatures in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Creatures instead of simple objects (optional)
	 */
	creatures(returnEntities = true) {

		return this.#getEntityList('creature', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an array of all players in the game.
	 *
	 * @param {Boolean} returnEntities If true, return an array of Players instead of simple objects (optional)
	 */
	players(returnEntities = true) {

		return this.#getEntityList('player', returnEntities);
	}

	/**
	 * Returns a promise that resolves to an object representing an entity in the game.
	 *
	 * @param {String} name The desired entity's name
	 */
	getEntity(name) {

		return this.#getEntityObject('entity', name);
	}

	/**
	 * Returns a promise that resolves to an object representing a place in the game.
	 *
	 * @param {String} name The desired place's name
	 */
	getPlace(name) {

		return this.#getEntityObject('place', name);
	}

	/**
	 * Returns a promise that resolves to an object representing a thing in the game.
	 *
	 * @param {String} name The desired thing's name
	 */
	getThing(name) {

		return this.#getEntityObject('thing', name);
	}

	/**
	 * Returns a promise that resolves to an object representing a being in the game.
	 *
	 * @param {String} name The desired being's name
	 */
	getBeing(name) {

		return this.#getEntityObject('being', name);
	}

	/**
	 * Returns a promise that resolves to an object representing a room in the game.
	 *
	 * @param {String} name The desired room's name
	 */
	getRoom(name) {

		return this.#getEntityObject('room', name);
	}

	/**
	 * Returns a promise that resolves to an object representing a TObject in the game.
	 *
	 * @param {String} name The desired object's name
	 */
	getObject(name) {

		return this.#getEntityObject('object', name);
	}

	/**
	 * Returns a promise that resolves to an object representing a creature in the game.
	 *
	 * @param {String} name The desired creature's name
	 */
	getCreature(name) {

		return this.#getEntityObject('creature', name);
	}

	/**
	 * Returns a promise that resolves to an object representing a player in the game.
	 *
	 * @param {String} name The desired player's name
	 */
	getPlayer(name) {

		return this.#getEntityObject('player', name);
	}

	/**
	 * Returns a promise that resolves to an instance of Player representing
	 * the new player in the game.
	 *
	 * @param {String} name The new player's name
	 */
	createPlayer(name) {

		return new Promise((resolve, reject) => {

			this.#trogdord.makeRequest({
				method: "post",
				scope: "player",
				args: {
					game_id: this.#id,
					name: name
				}
			}).then(response => {

				if (200 != response.status) {

					let error = new Error(response.message);

					error.status = response.status;
					reject(error);
				}

				resolve(new this.#EntityTypes['player'](this, name));

			}).catch(error => {
				reject(error);
			});
		});
	}
};

module.exports = Game;