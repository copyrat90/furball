import os
import struct
from dataclasses import dataclass

from chipchune.furnace.module import FurnaceModule, FurnacePattern
from chipchune.furnace.data_types import FurnaceRow, ChipInfo
from chipchune.furnace.enums import ChipType, Note

from fb_exceptions import *


class FurballModule:
    def __init__(self, furnace_module_path: str):
        module = FurnaceModule(furnace_module_path)

        # Check if module is valid for Furball
        if len(module.chips.list) > TooManyChipsError.MAX_CHIPS:
            raise TooManyChipsError(len(module.chips.list))
        if len(module.instruments) > TooManyInstrumentsError.MAX_INSTRUMENTS:
            raise TooManyInstrumentsError(len(module.instruments))
        for chip in module.chips.list:
            if chip.type != ChipType.GB:
                raise UnsupportedChipTypeError(chip.type)

        self.module = module

        if len(module.subsongs) > 1:
            print(f'[WARN] "{os.path.basename(furnace_module_path)}": ignored {len(module.subsongs) - 1} subsong(s)')


    def write_file(self, output_file_path: str, c_array_name: str):
        output_file_basename = os.path.basename(output_file_path)

        try:
            with open(output_file_path, "w") as f:
                total_used_bytes = 0
                f.write("#include <stddef.h>" + "\n")
                f.write("#include <stdint.h>" + "\n\n")

                gb_chips = list(filter(lambda ci: ci.type == ChipType.GB, self.module.chips.list))
                if len(gb_chips) > 1:
                    raise TooManyGBChipsError(len(gb_chips))
                gb_chip: ChipInfo = gb_chips[0]

                # invert ch3 wavetable
                is_gba = gb_chip.flags["chipType"] == 3 if "chipType" in gb_chip.flags else False
                raw_invert = gb_chip.flags["invertWave"] if "invertWave" in gb_chip.flags else True
                real_invert: bool = not (is_gba ^ raw_invert)

                def inv(val: int) -> int:
                    assert 0 <= val <= 0xF
                    return 0xF - val if real_invert else val

                # wavetables
                for w_idx, wavetable in enumerate(self.module.wavetables):
                    f.write(f"static const uint32_t {c_array_name}_ch3_wt{w_idx}[] = {{" + "\n")

                    width = wavetable.meta.width
                    height = wavetable.meta.height
                    if height != 16 or (width != 32 and width != 64):
                        UnsupportedCh3WavetableSizeError(width, height)
                    if max(wavetable.data) >= 16:
                        UnsupportedCh3WavetableValueError(max(wavetable.data))

                    f.write(f"{width}, // width")
                    total_used_bytes += 4

                    for i in range(width // 8):
                        if i % 4 == 0:
                            f.write(f"\n")
                        # 4 byte little endian
                        data = wavetable.data
                        val  = (inv(data[8*i + 0]) <<  4) | (inv(data[8*i + 1]) <<  0)
                        val |= (inv(data[8*i + 2]) << 12) | (inv(data[8*i + 3]) <<  8)
                        val |= (inv(data[8*i + 4]) << 20) | (inv(data[8*i + 5]) << 16)
                        val |= (inv(data[8*i + 6]) << 28) | (inv(data[8*i + 7]) << 24)
                        f.write(f"0x{val:08x},")
                        total_used_bytes += 4

                    f.write("\n" + "};" + "\n")

                # wavetables ptr array
                if not self.module.wavetables:
                    f.write(f"static const uint32_t *const *const {c_array_name}_ch3_wavetables = NULL;" + "\n")
                else:
                    f.write(f"static const uint32_t *const {c_array_name}_ch3_wavetables[] = {{")
                    for w_idx in range(len(self.module.wavetables)):
                        if w_idx % 4 == 0:
                            f.write(f"\n")
                        f.write(f"{c_array_name}_ch3_wt{w_idx},")
                        total_used_bytes += 4
                    f.write("\n" + "};" + "\n")

                # patterns
                @dataclass
                class PatternFlags:
                    vol: bool = False
                    note: bool = False
                    inst: bool = False
                    max_effects: int = 0

                    def to_number(self) -> int:
                        assert 0 <= self.max_effects <= 8
                        return (self.vol << 6) | (self.note << 5) | (self.inst << 4) | self.max_effects

                for channel in range(self.module.get_num_channels()):
                    # pattern data
                    for pattern in filter(lambda pt: pt.channel == channel and pt.subsong == 0, self.module.patterns):
                        # flags first
                        flags = PatternFlags()

                        for row in pattern.data:
                            flags.vol |= (row.volume != 0xFFFF)
                            flags.note |= (row.note != Note.__)
                            flags.inst |= (row.instrument != 0xFFFF)
                            effects_count = sum(1 for _ in filter(lambda fx: fx[0] != 0xFFFF,
                            row.effects))
                            if effects_count > flags.max_effects:
                                flags.max_effects = effects_count

                        f.write(
                            f"_Alignas(2) static const uint8_t {c_array_name}_ch{channel}_pt{pattern.index}[] = {{" + "\n"
                        )
                        f.write(
                            f"0x{flags.to_number():02x},0x00, // flags" + "\n"
                        )
                        total_used_bytes += 2

                        for row_idx, row in enumerate(pattern.data):
                            ba = bytearray()
                            if flags.vol:
                                # vol `0xFFFF`: empty command
                                ba += struct.pack("<H", row.volume)
                            if flags.note:
                                note_octave = self.__get_note_octave(row)
                                # TODO: raise UnsupportedNoteError here
                                ba += struct.pack("<B", note_octave)
                            if flags.inst:
                                # inst `0xFF`: empty instrument
                                ba += struct.pack("<B", 0xFF if row.instrument == 0xFFFF else row.instrument)
                            if flags.max_effects > 0:
                                fx_idx = -1
                                for fx_idx, fx in enumerate(filter(lambda fx: fx[0] != 0xFFFF, row.effects)):
                                    assert fx_idx < flags.max_effects
                                    ba += struct.pack("<B", fx[0]) # effect
                                    ba += struct.pack("<B", 0x00 if fx[1] == 0xFFFF else fx[1]) # value
                                empty_fx_cnt = flags.max_effects - (fx_idx + 1)
                                # fx `0xAAAA`: empty effect
                                ba += b"\xAA\xAA" * empty_fx_cnt

                            assert len(ba) == (2 * flags.vol + flags.note + flags.inst + 2 * flags.max_effects)

                            for b in ba:
                                f.write(f"0x{b:02x},")
                            if len(ba) > 0:
                                f.write("\n")
                            total_used_bytes += len(ba)

                        f.write("};" + "\n")

                    # order table
                    order: List[int] = self.module.subsongs[0].order[channel]

                    f.write(
                        f"static const uint8_t *const {c_array_name}_ch{channel}_ord[] = {{"
                    )
                    for i, num in enumerate(order):
                        if i % 4 == 0:
                            f.write("\n")
                        f.write(f"{c_array_name}_ch{channel}_pt{num},")
                        total_used_bytes += 4

                    f.write("\n" + "};" + "\n")

            print(f'"{output_file_basename}" ({total_used_bytes} bytes in ROM)')

        except (OSError, IOError) as e:
            print(f'"{output_file_basename}" convert FAILED!')
            raise
        except Exception as e:
            print(f'"{output_file_basename}" convert FAILED!')
            if os.path.exists(output_file_path) and os.path.isfile(output_file_path):
                os.remove(output_file_path)
            raise


    def __get_note_octave(self, row: FurnaceRow) -> int:
        """
        Return a note + octave number according to furnace dev157 pattern format, but in 1 byte (empty=`0xFF`).
        https://github.com/tildearrow/furnace/blob/master/papers/format.md#pattern-157
        """
        if row.note == Note.__:
            return 0xFF
        if row.note == Note.OFF:
            return 180
        if row.note == Note.OFF_REL:
            return 181
        if row.note == Note.REL:
            return 182

        return 12 * (5 + row.octave) + row.note.value % 12


if __name__ == "__main__":
    import argparse
    import sys

    parser = argparse.ArgumentParser(
        description="convert Furnace modules into Furball binary format."
    )
    parser.add_argument(
        "--input", default=None, required=True, help="input Furnace `*.fur` file path"
    )
    parser.add_argument(
        "--output", default=None, required=False, help="output Furball file path"
    )
    parser.add_argument(
        "--c-array-name", default=None, required=False, help="C array identifier"
    )

    args = parser.parse_args()

    # Use `input_filename.c` as output file path
    if not args.output:
        args.output = os.path.splitext(args.input)[0] + ".c"
    # Use `output_filename` as C array name
    if not args.c_array_name:
        args.c_array_name = os.path.splitext(os.path.basename(args.output))[0]
    
    fb_module = FurballModule(args.input)
    fb_module.write_file(args.output, args.c_array_name)
