from chipchune.furnace.enums import ChipType, Note


class TooManyChipsError(Exception):
    MAX_CHIPS = 1

    def __init__(self, chips: int):
        super().__init__(f"Too many {chips=} (max {self.MAX_CHIPS})")


class TooManyInstrumentsError(Exception):
    MAX_INSTRUMENTS = 254

    def __init__(self, instruments: int):
        super().__init__(f"Too many {instruments=} (max {self.MAX_INSTRUMENTS})")


class UnsupportedChipTypeError(Exception):
    def __init__(self, chip: ChipType):
        super().__init__(f'Unsupported {chip=} (only supports "Game Boy" currently)')


class UnsupportedNoteError(Exception):
    def __init__(self, note: Note, octave: int, pattern: int, row: int):
        super().__init__(f'Unsupported note "{note} (oct {octave})" in {pattern=}, {row=}')
