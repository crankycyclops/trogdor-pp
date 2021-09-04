
# Change Log

## [0.90.0] - In Development

### Added

- Added room connection descriptions ([Connection descriptions feature #72 ](https://github.com/crankycyclops/trogdor-pp/pull/72)), a more flexible way to generate room descriptions that can be updated dynamically when those connections change. This new feature shouldn't have any impact on older projects.

### Changed

- Reimplemented the items in a Being's inventory using std::weak_ptr. Any code that calls Being::getInventoryObjects() or Being::getInventoryObjectsByName() will have to be adjusted to take this change into account.
- Game::removeEntity() now enforces a more complete set of rules and no longer returns a boolean value (instead, it throws an exception if anything goes wrong.)

### Fixed

(No fixes)

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