# Furball

Furball is a sound library for the Game Boy Advance.\
It lets you play the [Furnace](https://github.com/tildearrow/furnace) DMG + PCM songs on the GBA.

> TODO


## Usage

### Building the library

Setup [devkitARM](https://devkitpro.org/wiki/Getting_Started), and call `make -j$(nproc)`\
It'll build the `lib/libfurball.a`

#### Makefile options

If you change any Makefile option, you need to run `make clean` before running `make`.
Also, note that `1` is just an arbitrary value;  It is enabled when the value is not empty.

* `FB_MGBA_LOG := 1` : enables mGBA logging.


### Using the converter

> TODO
