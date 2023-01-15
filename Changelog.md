
# Change Log

## [0.91.1] - 2023-01-14

### Changed

- Entities no longer have their own Lua states, which is wasteful. Instead, there is only one shared state per game.

## [0.91.0] - 2022-03-06

### Added

- trogdord can now authenticate with redis using an optional username and password
- trogdord can now also be configured via environment variables, which take precedence over trogdord.ini

### Changed

- The timer period (tick interval) can now be changed after the timer's started and can be configured via a game definition file
- trogdord now only listens on 127.0.0.1 or ::1 by default

### Fixed

- Added missing validation for ALLOCATE_RESOURCE AST operation

## [0.90.0] - 2022-01-31

### Added

- Added room connection descriptions ([Connection descriptions feature #72 ](https://github.com/crankycyclops/trogdor-pp/pull/72)), a more flexible way to generate room descriptions that can be updated dynamically when those connections change. This new feature shouldn't have any impact on older projects.
- Wrote two new Lua methods, Being:insertIntoInventory() and Being:removeFromInventory() to replace functionality previously served by the now removed method Thing:setLocation.
- Implemented Lua function Resource.new() to create new Resources from within Lua
- Implemented Lua methods Game::getEntity(), Game:start(), Game:stop(), and Game::inProgress()
- Added LuaState::getLuaVersion(), which returns the version of Lua the core library was built against
- Added LuaEntity::err() to allow writing to an Entity's error stream
- Started writing unit tests for the LuaState class
- Unit tests are now built and run against Lua 5.1, 5.2, and 5.3
- trogdord can now listen on IPv6 as well as IPv4, and can also be configured to listen for requests only from certain IP addresses

### Changed

- Reimplemented the items in a Being's inventory using std::weak_ptr. Any code that calls Being::getInventoryObjects() or Being::getInventoryObjectsByName() will have to be adjusted to take this change into account.
- Game::removeEntity() now enforces a more complete set of rules and no longer returns a boolean value (instead, it throws an exception if anything goes wrong.)
- Object::setOwner() is now protected and an Object can only be inserted into a Being's inventory by calling Being::insertIntoInventory().
- Thing::setLocation() is now protected and a Thing can only be inserted into a Place by calling Place::insertThing().
- Removed Lua method Thing:setLocation().
- Many formerly private members and methods in trogdor::LuaState are now protected to make unit testing easier

### Fixed

- It turns out that the Tangible and Resource types weren't being registered with Lua. This was uncovered by my new unit tests and has been fixed.
- Fixed a bug that affected the way the Lua Resource type was registered
- Did some minor refactoring and code cleanup

## [0.80.0] - 2021-07-15

### Added

- Added extension loading for custom functionality to trogdord on platforms that support libdl.

### Changed

- Split the build into multiple CMake files, making things more modular (I'm hoping to eventually spin each item out into its own repository)

## [0.70.0] - 2021-06-22

### Added

- Introduced the SQLite3 serialization format and improves trogdord's handling of dumped games.

## [0.60.0] - 2021-03-16

### Added

- Introduced state features, making it possible to dump and restore game state to disk.

### Changes

- Many changes that I didn't track at the time

### Fixes

- Many fixes that I also didn't track at the time

## [0.50.2] - Before 2021-03-16

This was the version of the engine right before I started tracking versions and their changes. If you're really curious, you can look through the very lengthy commit log for more history.
