# Furball

Furball is a sound library for the Game Boy Advance.\
It lets you play the [Furnace](https://github.com/tildearrow/furnace) DMG + PCM music on the GBA.

> TODO


## Usage

### Building the library

Setup [devkitARM](https://devkitpro.org/wiki/Getting_Started), and call `make -j$(nproc)`\
It'll build the `lib/libfurball.a`

#### Makefile options

If you change any Makefile option, you need to run `make clean` before running `make`.
Also, note that `1` is just an arbitrary value;  It is enabled when the value is not empty.

* `FB_MGBA_LOG_ENABLED := 1`
    + Enables mGBA logging.
* `FB_MGBA_LOG_MAY_FATAL_CRASH := 1`
    + If enabled, `FB_LOG_MAY_FATAL(...)` is FATAL (crash). Otherwise, it's ERROR (not crash).

### Using the converter

> TODO
