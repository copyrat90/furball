from chipchune.furnace.enums import ChipType, InstrumentType, Note


class TooManyChipsError(Exception):
    MAX_CHIPS = 1

    def __init__(self, chips: int):
        super().__init__(f"Too many {chips=} (max {self.MAX_CHIPS})")


class TooManyGBChipsError(Exception):
    def __init__(self, gb_chips: int):
        super().__init__(f"Too many {gb_chips=} (max 1)")


class TooManyInstrumentsError(Exception):
    MAX_INSTRUMENTS = 254

    def __init__(self, instruments: int):
        super().__init__(f"Too many {instruments=} (max {self.MAX_INSTRUMENTS})")


class TooManyWavetablesError(Exception):
    MAX_WAVETABLES = 255

    def __init__(self, wavetables: int):
        super().__init__(f"Too many {wavetables=} (max {self.MAX_WAVETABLES})")


class UnsupportedChipTypeError(Exception):
    def __init__(self, chip: ChipType):
        super().__init__(f'Unsupported {chip=} (only supports "Game Boy" currently)')


class UnsupportedInstrumentTypeError(Exception):
    def __init__(self, instrument_type: InstrumentType):
        super().__init__(f"Unsupported {instrument_type=}")


class UnsupportedNoteError(Exception):
    def __init__(self, note: Note, octave: int, pattern: int, row: int):
        super().__init__(
            f'Unsupported note "{note} (oct {octave})" in {pattern=}, {row=}'
        )


class UnsupportedWavetableSizeError(Exception):
    def __init__(self, wave_idx: int, width: int, height: int):
        super().__init__(
            f"Size of {wave_idx=} is {width}x{height} (only supports 32x16)"
        )


class UnsupportedWavetableValueError(Exception):
    def __init__(self, height_value: int):
        super().__init__(f"Invalid wavetable {height_value=}")
