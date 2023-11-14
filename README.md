# Furball

Furball is a sound driver library for the Game Boy Advance.\
It lets you play the [Furnace](https://github.com/tildearrow/furnace) DMG(+PCM) music on the GBA.

Well... not yet.


## Development status

In short: Heavily WIP, **expect breaking changes all the time.**

* Only very basic DMG Ch1/2 playback for now.
    + No effects, macros... nothing.
* No PCM support yet.


## Usage

### Building the library

Setup [devkitARM](https://devkitpro.org/wiki/Getting_Started), and call `make -j$(nproc)`\
It'll build the `lib/libfurball.a`

#### Makefile options

If you change any Makefile option, you need to run `make clean` before running `make`.\
Also, note that `1` is just an arbitrary value;  It is enabled when the value is not empty.

* `FB_MGBA_LOG_ENABLED := 1`
    + Enables mGBA logging.
    + This also enables `FB_ASSERT(...)`, as it uses mGBA log level FATAL under the hood.
* `FB_MGBA_LOG_MAY_FATAL_CRASH := 1`
    + If enabled, `FB_LOG_MAY_FATAL(...)` is FATAL (crash). Otherwise, it's ERROR (not crash).

### Using the converter

You need Python 3.8+ to use the converter reliably.

```bash
# simple usage, it will create "my_song.c" in the same directory.
python converter/fb_converter.py --input my_song.fur
# specify output file path
python converter/fb_converter.py --input my_song.fur --output pre_existing_dir/result.c
# change the name of `const fb_music` C variable
python converter/fb_converter.py --input my_song.fur --c-var-name song_title
```

### Playing the music

The converter's `*.c` output will look similar to this,\
so you can declare the variable as `extern` to reference your song in your source.
```c
#include "fb_music.h"
...
// declare this variable as extern in your source.
extern const fb_music my_song;
...
```

In order to play this, you'll need to call `fb_update_vblank()` in each VBlank.\
I'm using [ugba](https://github.com/AntonioND/libugba)'s interrupt handler for this example:
```c
#include "furball.h"
#include "ugba/ugba.h"

extern const fb_music my_song;

int main(void) {
    // Initialize Furball with default settings.
    // See `struct fb_init_settings` in `include/fb_settings.h`
    fb_init(NULL);

    // Start playing with loop enabled.
    // See `enum fb_loop_setting` in `include/fb_settings.h`
    fb_play(&my_song, FB_LOOP_SETTING_LOOP);

    // Add `fb_update_vblank()` as VBlank ISR (ugba)
    IRQ_Init();
    IRQ_SetHandler(IRQ_VBLANK, fb_update_vblank);
    IRQ_Enable(IRQ_VBLANK);

    while (1)
        SWI_VBlankIntrWait();
}
```
