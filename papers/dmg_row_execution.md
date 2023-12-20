# DMG channels - Row execution

## Terminology

* *Retrigger*
    + Write the register: retrigger
        * `FB_REG_SNDxFREQ = ... | FB_SNDxFREQ_RETRIG;`

* *Initialize the envelop*
    + Write the register: *Envelop Volume, Envelop Length, Sound Length, Envelop Direction* with the GB instrument's.
    + Do not confuse this with *retriggering*, they are different.
    + Interesting quirk: Setting *Envelop Volume* to `0` stops the sound immediately.
        * https://gbdev.io/pandocs/Audio_Registers.html#ff12--nr12-channel-1-volume--envelope
          > Writes to this register while the channel is on require retriggering it afterwards. If the write turns the channel off, retriggering is not necessary (it would do nothing).
            + This pandocs note is partially true. At least for *Envelop Length*, [the behavior is platform dependent.](https://github.com/mgba-emu/mgba/issues/3067#issuecomment-1862516883)\
              Setting *Envelop Length* without retrigger works on GBA, and that's what Furnace does if you put an instrument without a note in a row.\
              But while it works on GBA, it doesn't work on NDS.


## Furnace - Row behavior

* Changing an instrument *initializes the envelop*.
    + Not changing the instrument (same instument value) does nothing.

* Putting the note *retriggers* the channel.
    + It doesn't *initialize the envelop*, by default.
    + It DOES *initialize the envelop*, if `Initialize envelop on every note` option is turned on for currently applied instrument.

* Changing the volume *initializes the envelop*.
    + Changed volume is immediately applied for Channel 3 (WAV)
    + Changed volume is not applied until *retrigger* for Channel 1/2/4 (PUx, NOI)

* Changing the duty cycle *initializes the envelop*.
