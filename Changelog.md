
# Change Log

## [0.90.0] - In Development

### Added

- Added room connection descriptions ([Connection descriptions feature #72 ](https://github.com/crankycyclops/trogdor-pp/pull/72)), a more flexible way to generate room descriptions that can be updated dynamically when those connections change. This new feature shouldn't have any impact on older projects.
- Wrote two new Lua methods, Being:insertIntoInventory() and Being:removeFromInventory() to replace functionality previously served by the now removed method Thing:setLocation.
- Implemented Lua function Resource.new() to create new Resources from within Lua
- Wrote unit tests for the LuaState class
- Unit tests are now built and run against Lua 5.1, 5.2, and 5.3

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