import os
import logging
import struct
from dataclasses import dataclass, field
from typing import List, Tuple, Union, TextIO

from chipchune.furnace.module import FurnaceModule, FurnacePattern
from chipchune.furnace.enums import (
    ChipType,
    InstrumentType,
    Note,
    GBHwCommand,
    MacroCode,
    MacroItem,
)

from chipchune.furnace.data_types import (
    FurnaceRow,
    ChipInfo,
    GBHwSeq,
    InsFeatureGB,
    InsFeatureName,
    InsFeatureMacro,
    InsFeatureWaveSynth,
    InsFeatureAmiga,
    SingleMacro,
)

from fb_exceptions import *


class FurballModule:
    def __init__(self, furnace_module_path: str):
        module = FurnaceModule(furnace_module_path)

        # Check if module is valid for Furball
        song = module.subsongs[0]
        if song.timing.clock_speed != 60.0:
            raise UnsupportedTickRateError(song.timing.clock_speed)
        if song.timing.timebase != 1:
            raise InvalidTimeBaseError(song.timing.timebase)
        if len(module.chips.list) > TooManyChipsError.MAX_CHIPS:
            raise TooManyChipsError(len(module.chips.list))
        if len(module.instruments) > TooManyInstrumentsError.MAX_INSTRUMENTS:
            raise TooManyInstrumentsError(len(module.instruments))
        for chip in module.chips.list:
            if chip.type != ChipType.GB:
                raise UnsupportedChipTypeError(chip.type)
        gb_chips = list(filter(lambda ci: ci.type == ChipType.GB, module.chips.list))
        if len(gb_chips) > 1:
            raise TooManyGBChipsError(len(gb_chips))

        self.module = module

        if len(module.subsongs) > 1:
            logging.warning(
                f'"{os.path.basename(module.file_name)}": ignored {len(module.subsongs) - 1} subsong(s)'
            )

    def write_file(self, output_file_path: str, c_var_name: str) -> int:
        """
        :returns: total used bytes
        """
        output_file_basename = os.path.basename(output_file_path)

        try:
            with open(output_file_path, "w") as f:
                total_used_bytes = 0
                song = self.module.subsongs[0]

                f.write('#include "fb_music.h"' + "\n\n")
                f.write("#include <stddef.h>" + "\n\n")

                f.write(f"// declare this variable as extern in your source." + "\n")
                f.write(f"extern const fb_music {c_var_name};" + "\n\n\n\n")

                f.write("// ======== internal data ========" + "\n\n")

                # instruments
                for inst_idx, inst in enumerate(self.module.instruments):
                    inst_kind = None
                    if inst.meta.type == InstrumentType.GB:
                        inst_kind = "GB"
                    elif inst.meta.type == InstrumentType.AMIGA:
                        inst_kind = "SAMPLE"
                    else:
                        raise UnsupportedInstrumentTypeError(inst.meta.type)

                    has_gb: bool = False
                    macros_count: int = None
                    has_wave_synth: bool = False
                    has_sample: bool = False

                    if inst.meta.type not in (InstrumentType.GB, InstrumentType.AMIGA):
                        logging.warning(
                            f'"{os.path.basename(self.module.file_name)}": ignored instrument 0x{inst_idx:02X}={inst}'
                        )
                    else:
                        for feature in inst.features:
                            if type(feature) == InsFeatureName:
                                pass

                            elif type(feature) == InsFeatureGB:
                                assert not has_gb
                                has_gb = True
                                gb: InsFeatureGB = feature

                                # GB hardware sequence
                                if gb.hw_seq:
                                    f.write(
                                        f"static const fb_gb_hw_cmd {c_var_name}_inst{inst_idx:02X}_gb_hw_seq[] = {{"
                                        + "\n"
                                    )
                                    for hw_cmd in gb.hw_seq:
                                        f.write("{.kind=FB_GB_HW_CMD_KIND_")
                                        if hw_cmd.command == GBHwCommand.ENVELOPE:
                                            f.write("ENVELOP," + "\n")
                                            f.write(".envelop={" + "\n")
                                            f.write(
                                                f".volume={(hw_cmd.data[0] & 0b11110000) >> 4},"
                                                + "\n"
                                            )
                                            f.write(
                                                f".envelop_length={hw_cmd.data[0] & 0b111},"
                                                + "\n"
                                            )
                                            f.write(
                                                f".sound_length={hw_cmd.data[1]},"
                                                + "\n"
                                            )
                                            f.write(
                                                f".direction_up={str((hw_cmd.data[0] & 0b1000) != 0).lower()},"
                                                + "\n"
                                            )
                                            f.write("},")
                                        elif hw_cmd.command == GBHwCommand.SWEEP:
                                            f.write("SWEEP," + "\n")
                                            f.write(".sweep={" + "\n")
                                            f.write(
                                                f".shift={hw_cmd.data[0] & 0b111},"
                                                + "\n"
                                            )
                                            f.write(
                                                f".speed={(hw_cmd.data[0] & 0b1110000) >> 4},"
                                                + "\n"
                                            )
                                            f.write(
                                                f".direction_down={str((hw_cmd.data[0] & 0b1000) != 0).lower()},"
                                                + "\n"
                                            )
                                            f.write("},")
                                        elif hw_cmd.command == GBHwCommand.WAIT:
                                            f.write("WAIT," + "\n")
                                            f.write(".wait={" + "\n")
                                            f.write(
                                                f".length={hw_cmd.data[0]+1}," + "\n"
                                            )
                                            f.write("},")
                                        elif hw_cmd.command == GBHwCommand.WAIT_REL:
                                            f.write("WAIT_FOR_RELEASE," + "\n")
                                        elif hw_cmd.command == GBHwCommand.LOOP:
                                            f.write("LOOP," + "\n")
                                            f.write(".loop={" + "\n")
                                            f.write(
                                                f".position={(hw_cmd.data[1] << 8) | (hw_cmd.data[0])},"
                                                + "\n"
                                            )
                                            f.write("},")
                                        elif hw_cmd.command == GBHwCommand.LOOP_REL:
                                            f.write("LOOP_UNTIL_RELEASE," + "\n")
                                            f.write(".loop={" + "\n")
                                            f.write(
                                                f".position={(hw_cmd.data[1] << 8) | (hw_cmd.data[0])},"
                                                + "\n"
                                            )
                                            f.write("},")
                                        else:
                                            assert (
                                                False
                                            ), f"Invalid GBHwCommand={hw_cmd.command}"
                                        f.write("}," + "\n")
                                        total_used_bytes += 4 + 4
                                    f.write("};" + "\n")
                                else:
                                    f.write(
                                        f"static const fb_gb_hw_cmd *const {c_var_name}_inst{inst_idx:02X}_gb_hw_seq = NULL;"
                                        + "\n"
                                    )

                                # GB instrument
                                f.write(
                                    f"static const fb_inst_gb {c_var_name}_inst{inst_idx:02X}_gb = {{"
                                    + "\n"
                                )
                                f.write(f".initial_volume={gb.env_vol}," + "\n")
                                f.write(f".envelop_length={gb.env_len}," + "\n")
                                f.write(f".sound_length={gb.sound_len}," + "\n")
                                f.write(
                                    f".envelop_direction_up={str(bool(gb.env_dir)).lower()},"
                                    + "\n"
                                )
                                f.write(
                                    f".always_init_envelop={str(bool(gb.always_init)).lower()},"
                                    + "\n"
                                )
                                f.write(
                                    f".software_envelop={str(bool(gb.soft_env)).lower()},"
                                    + "\n"
                                )
                                f.write(
                                    f".hardware_sequence_length={len(gb.hw_seq)},"
                                    + "\n"
                                )
                                f.write(
                                    f".hardware_sequence={c_var_name}_inst{inst_idx:02X}_gb_hw_seq,"
                                    + "\n"
                                )
                                f.write("};" + "\n")
                                total_used_bytes += 12

                            elif type(feature) == InsFeatureMacro:
                                assert macros_count == None
                                macros_count = len(feature.macros)

                                # write single macro
                                for m_idx, macro in enumerate(feature.macros):
                                    macro_darr_type: str = ""
                                    if macro.kind in (
                                        MacroCode.VOL,
                                        MacroCode.DUTY,
                                        MacroCode.WAVE,
                                    ):
                                        macro_darr_type = "uint8_t"
                                    elif macro.kind in (
                                        MacroCode.ARP,
                                        MacroCode.PAN_L,
                                        MacroCode.PAN_R,
                                    ):
                                        macro_darr_type = "int8_t"
                                    elif macro.kind == MacroCode.PITCH:
                                        macro_darr_type = "int16_t"
                                    elif macro.kind == MacroCode.PHASE_RESET:
                                        macro_darr_type = "bool"
                                    else:
                                        assert False, f"Invalid {macro.kind=}"

                                    macro_darr_type_size = (
                                        2
                                        if macro_darr_type == "int16_t"
                                        or macro.kind == MacroCode.ARP
                                        else 1
                                    )

                                    # write data
                                    macro_list = self.__convert_macro_data(macro.data)
                                    f.write(
                                        f"static const {macro_darr_type} {c_var_name}_inst{inst_idx:02X}_macro{m_idx}_data[] = {{"
                                    )
                                    for k, num in enumerate(macro_list.data):
                                        if k % 16 == 0:
                                            f.write("\n")
                                        if macro.kind == MacroCode.ARP:
                                            neg: bool = num < 0
                                            num = abs(num)
                                            fixed_arp = bool(num & 0x40000000)
                                            f.write(
                                                f"{(num & 0xFF) * (-1 if neg else 1)},"
                                            )
                                            f.write(f"{str(fixed_arp).lower()},")
                                        else:
                                            f.write(f"{num},")

                                    f.write("\n" + "};" + "\n")
                                    total_used_bytes += (
                                        len(macro_list.data) * macro_darr_type_size
                                    )

                                    # write macro
                                    f.write(
                                        f"static const fb_inst_macro {c_var_name}_inst{inst_idx:02X}_macro{m_idx} = {{"
                                        + "\n"
                                    )
                                    f.write(
                                        f".kind=FB_MACRO_KIND_{str(macro.kind)}," + "\n"
                                    )
                                    f.write(f".mode={macro.mode}," + "\n")
                                    f.write(f".length={len(macro_list.data)}," + "\n")
                                    f.write(
                                        f".loop_pos={macro_list.loop_pos if macro_list.loop_pos != -1 else 0xFF},"
                                        + "\n"
                                    )
                                    f.write(
                                        f".release_pos={macro_list.release_pos if macro_list.release_pos != -1 else 0xFF},"
                                        + "\n"
                                    )
                                    f.write(f".delay={macro.delay}," + "\n")
                                    f.write(f".speed={macro.speed}," + "\n")
                                    f.write(
                                        f".data={c_var_name}_inst{inst_idx:02X}_macro{m_idx}_data,"
                                        + "\n"
                                    )
                                    f.write("};" + "\n")
                                    total_used_bytes += 16

                                # write macro array
                                f.write(
                                    f"static const fb_inst_macro {c_var_name}_inst{inst_idx:02X}_macros[] = {{"
                                )
                                for m_idx in range(macros_count):
                                    if m_idx % 4 == 0:
                                        f.write("\n")
                                    f.write(
                                        f"{c_var_name}_inst{inst_idx:02X}_macro{m_idx},"
                                    )
                                f.write("\n" + "};" + "\n")
                                # macros should be moved to array, so no need to increase `total_used_bytes` here

                            elif type(feature) == InsFeatureWaveSynth:
                                assert not has_wave_synth
                                has_wave_synth = True

                                wave_synth: InsFeatureWaveSynth = feature
                                if wave_synth.enabled:
                                    f.write(
                                        f"static const fb_inst_wave_synth {c_var_name}_inst{inst_idx:02X}_wave_synth = {{"
                                        + "\n"
                                    )
                                    f.write(
                                        f".kind=FB_WAVE_SYNTH_KIND_{str(wave_synth.effect)},"
                                        + "\n"
                                    )
                                    f.write(
                                        f".global={str(wave_synth.global_effect).lower()},"
                                        + "\n"
                                    )
                                    f.write(
                                        f".wave_1={wave_synth.wave_indices[0]}," + "\n"
                                    )
                                    f.write(
                                        f".wave_2={wave_synth.wave_indices[1]}," + "\n"
                                    )
                                    f.write(
                                        f".rate_divider={wave_synth.rate_divider},"
                                        + "\n"
                                    )
                                    f.write(f".speed={wave_synth.speed}," + "\n")
                                    f.write(f".amount={wave_synth.params[0]}," + "\n")
                                    f.write(f".power={wave_synth.params[1]}," + "\n")
                                    f.write("};" + "\n")
                                    total_used_bytes += 11

                            # TODO: Feature - sample
                            # elif type(feature) == InsFeatureAmiga:
                            #     assert not has_sample
                            #     has_sample = True
                            #     pass

                            else:
                                logging.warning(
                                    f'"{os.path.basename(self.module.file_name)}": ignored instrument 0x{inst_idx:02X}\'s {feature=}'
                                )

                    # write single `fb_instrument`
                    if macros_count == None:
                        macros_count = 0
                    f.write(
                        f"static const fb_instrument {c_var_name}_inst{inst_idx:02X} = {{"
                        + "\n"
                    )
                    f.write(f".kind=FB_INST_KIND_{inst_kind}," + "\n")
                    f.write(f".macros_count={macros_count}," + "\n")
                    f.write(
                        ".gb="
                        + (
                            "NULL"
                            if not has_gb
                            else f"&{c_var_name}_inst{inst_idx:02X}_gb"
                        )
                        + ",\n"
                    )
                    f.write(
                        ".macros="
                        + (
                            "NULL"
                            if macros_count <= 0
                            else f"{c_var_name}_inst{inst_idx:02X}_macros"
                        )
                        + ",\n"
                    )
                    f.write(
                        ".wave_synth="
                        + (
                            "NULL"
                            if not has_wave_synth
                            else f"&{c_var_name}_inst{inst_idx:02X}_wave_synth"
                        )
                        + ",\n"
                    )
                    f.write(
                        ".sample="
                        + (
                            "NULL"
                            if not has_sample
                            else f"&{c_var_name}_inst{inst_idx:02X}_sample"
                        )
                        + ",\n"
                    )
                    f.write("};" + "\n")
                    total_used_bytes += 24

                # write instruments array
                if self.module.instruments:
                    f.write(
                        f"static const fb_instrument {c_var_name}_instruments[] = {{"
                    )
                    for inst_idx in range(len(self.module.instruments)):
                        if inst_idx % 4 == 0:
                            f.write("\n")
                        f.write(f"{c_var_name}_inst{inst_idx:02X},")
                    f.write("\n" + "};" + "\n")
                    # instruments should be moved to array, so no need to increase `total_used_bytes` here
                else:
                    f.write(
                        f"static const fb_instrument *const {c_var_name}_instruments = NULL;"
                        + "\n"
                    )

                # invert wavetable
                gb_chip: ChipInfo = list(
                    filter(lambda ci: ci.type == ChipType.GB, self.module.chips.list)
                )[0]

                is_gba = (
                    gb_chip.flags["chipType"] == 3
                    if "chipType" in gb_chip.flags
                    else False
                )
                raw_invert = (
                    gb_chip.flags["invertWave"]
                    if "invertWave" in gb_chip.flags
                    else True
                )
                real_invert: bool = not (is_gba ^ raw_invert)

                def inv(val: int) -> int:
                    assert 0 <= val <= 0xF
                    return 0xF - val if real_invert else val

                # wavetables
                wavetable_dimensions: List[Tuple[int, int]] = []

                if len(self.module.wavetables) > TooManyWavetablesError.MAX_WAVETABLES:
                    raise TooManyWavetablesError(self.module.wavetables)

                for w_idx, wavetable in enumerate(self.module.wavetables):
                    f.write(f"static const uint32_t {c_var_name}_wt{w_idx}_data[] = {{")

                    width = wavetable.meta.width
                    height = wavetable.meta.height
                    if width != 32 or height != 16:
                        raise UnsupportedWavetableSizeError(w_idx, width, height)
                    if max(wavetable.data) >= 16:
                        raise UnsupportedWavetableValueError(max(wavetable.data))

                    wavetable_dimensions.append((width, height))

                    for i in range(width // 8):
                        if i % 4 == 0:
                            f.write(f"\n")
                        # 4 byte little endian
                        data = wavetable.data
                        val = (inv(data[8 * i + 0]) << 4) | (inv(data[8 * i + 1]) << 0)
                        val |= (inv(data[8 * i + 2]) << 12) | (
                            inv(data[8 * i + 3]) << 8
                        )
                        val |= (inv(data[8 * i + 4]) << 20) | (
                            inv(data[8 * i + 5]) << 16
                        )
                        val |= (inv(data[8 * i + 6]) << 28) | (
                            inv(data[8 * i + 7]) << 24
                        )
                        f.write(f"0x{val:08x},")
                        total_used_bytes += 4

                    f.write("\n" + "};" + "\n")

                # wavetables ptr array
                if not self.module.wavetables:
                    f.write(
                        f"static const fb_wavetable *const {c_var_name}_wavetables = NULL;"
                        + "\n"
                    )
                else:
                    f.write(
                        f"static const fb_wavetable {c_var_name}_wavetables[] = {{"
                        + "\n"
                    )
                    for w_idx in range(len(self.module.wavetables)):
                        f.write(f"{{.width={wavetable_dimensions[w_idx][0]}, ")
                        f.write(f".height={wavetable_dimensions[w_idx][1]}, ")
                        f.write(f".data={c_var_name}_wt{w_idx}_data}}," + "\n")
                        total_used_bytes += 8
                    f.write("};" + "\n")

                # patterns
                @dataclass
                class PatternFlags:
                    vol: bool = False
                    note: bool = False
                    inst: bool = False
                    max_effects: int = 0

                    def empty(self) -> bool:
                        return (
                            not self.vol
                            and not self.note
                            and not self.inst
                            and not self.max_effects
                        )

                for channel in range(self.module.get_num_channels()):
                    # pattern data
                    for pattern in filter(
                        lambda pt: pt.channel == channel and pt.subsong == 0,
                        self.module.patterns,
                    ):
                        # flags first
                        flags = PatternFlags()

                        for row in pattern.data:
                            flags.vol |= row.volume != 0xFFFF
                            flags.note |= row.note != Note.__
                            flags.inst |= row.instrument != 0xFFFF
                            effects_count = sum(
                                1
                                for _ in filter(lambda fx: fx[0] != 0xFFFF, row.effects)
                            )
                            if effects_count > flags.max_effects:
                                flags.max_effects = effects_count

                        if flags.empty():
                            f.write(
                                f"static const uint8_t *const {c_var_name}_ch{channel+1}_pt{pattern.index:02X}_data = NULL;"
                                + "\n"
                            )
                        else:
                            f.write(
                                f"_Alignas(2) static const uint8_t {c_var_name}_ch{channel+1}_pt{pattern.index:02X}_data[] = {{"
                                + "\n"
                            )

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
                                    ba += struct.pack(
                                        "<B",
                                        0xFF
                                        if row.instrument == 0xFFFF
                                        else row.instrument,
                                    )
                                if flags.max_effects > 0:
                                    fx_idx = -1
                                    for fx_idx, fx in enumerate(
                                        filter(lambda fx: fx[0] != 0xFFFF, row.effects)
                                    ):
                                        assert fx_idx < flags.max_effects
                                        ba += struct.pack("<B", fx[0])  # effect
                                        ba += struct.pack(
                                            "<B", 0x00 if fx[1] == 0xFFFF else fx[1]
                                        )  # value
                                    empty_fx_cnt = flags.max_effects - (fx_idx + 1)
                                    # fx `0xAAAA`: empty effect
                                    ba += b"\xAA\xAA" * empty_fx_cnt

                                assert len(ba) == (
                                    2 * flags.vol
                                    + flags.note
                                    + flags.inst
                                    + 2 * flags.max_effects
                                )

                                for b in ba:
                                    f.write(f"0x{b:02X},")
                                if len(ba) > 0:
                                    f.write("\n")
                                total_used_bytes += len(ba)

                            f.write("};" + "\n")

                        f.write(
                            f"static const fb_pattern {c_var_name}_ch{channel+1}_pt{pattern.index:02X} = {{"
                            + "\n"
                        )
                        f.write(f".has_volume={str(flags.vol).lower()}, ")
                        f.write(f".has_note={str(flags.note).lower()}, ")
                        f.write(f".has_instrument={str(flags.inst).lower()}," + "\n")
                        f.write(f".max_effects_count={flags.max_effects}, ")
                        f.write(
                            f".data={c_var_name}_ch{channel+1}_pt{pattern.index:02X}_data,"
                            + "\n"
                        )
                        f.write("};" + "\n")
                        total_used_bytes += 8

                    # order table
                    order: List[int] = song.order[channel]

                    f.write(
                        f"static const fb_pattern *const {c_var_name}_ch{channel+1}_ord[] = {{"
                    )
                    for i, num in enumerate(order):
                        if i % 4 == 0:
                            f.write("\n")
                        f.write(f"&{c_var_name}_ch{channel+1}_pt{num:02X},")
                        total_used_bytes += 4

                    f.write("\n" + "};" + "\n")

                # speeds
                f.write(f"static const uint8_t {c_var_name}_speeds[] = {{" + "\n")
                speeds = song.speed_pattern if song.speed_pattern else song.timing.speed
                for sp in speeds:
                    f.write(f"{sp},")
                f.write("\n" + "};" + "\n")
                total_used_bytes += len(speeds)

                # grooves
                for grv_idx, grv in enumerate(song.grooves):
                    if grv:
                        f.write(
                            f"static const uint8_t {c_var_name}_grv{grv_idx}_data[] = {{"
                            + "\n"
                        )
                        for grv_val in grv:
                            f.write(f"{grv_val},")
                        f.write("\n" + "};" + "\n")
                        total_used_bytes += len(grv)
                    else:
                        assert False, f"Groove 0x{grv_idx:02X} is empty"

                    f.write(
                        f"static const fb_groove {c_var_name}_grv{grv_idx} = {{" + "\n"
                    )
                    f.write(f".length={len(grv)}," + "\n")
                    f.write(f".data={c_var_name}_grv{grv_idx}_data," + "\n")
                    f.write("};" + "\n")
                    total_used_bytes += 5

                if song.grooves:
                    f.write(f"static const fb_groove {c_var_name}_grooves[] = {{")

                    for grv_idx in range(len(song.grooves)):
                        if grv_idx % 4 == 0:
                            f.write("\n")
                        f.write(f"{c_var_name}_grv{grv_idx},")
                    f.write("\n" + "};" + "\n")
                    # no need to add `total_used_bytes` here either
                else:
                    f.write(
                        f"static const fb_groove *const {c_var_name}_grooves = NULL;"
                        + "\n"
                    )

                # fb_music
                f.write("\n\n")
                f.write("// ======== result ========" + "\n")
                f.write(f"const fb_music {c_var_name} = {{" + "\n")
                f.write(f".speeds={c_var_name}_speeds," + "\n")
                f.write(f".speeds_length={len(speeds)}," + "\n")
                f.write(
                    f".virtual_tempo_numerator={song.timing.virtual_tempo[0]}," + "\n"
                )
                f.write(
                    f".virtual_tempo_denominator={song.timing.virtual_tempo[1]}," + "\n"
                )
                f.write(f".grooves_count={len(song.grooves)}," + "\n")
                f.write(f".grooves={c_var_name}_grooves," + "\n")
                f.write(f".instruments_count={len(self.module.instruments)}," + "\n")
                f.write(f".wavetables_count={len(self.module.wavetables)}," + "\n")
                f.write(f".instruments={c_var_name}_instruments," + "\n")
                f.write(f".wavetables={c_var_name}_wavetables," + "\n")
                f.write(f".order_length={len(song.order[0])}," + "\n")
                f.write(f".pattern_length={song.pattern_length}," + "\n")
                for dmg_ch in range(4):
                    f.write(
                        f".ch{dmg_ch+1}_order={c_var_name}_ch{dmg_ch+1}_ord," + "\n"
                    )
                f.write("};" + "\n")
                total_used_bytes += 44

            logging.info(
                f'"{output_file_basename}" written ({total_used_bytes} bytes in ROM)'
            )

        except (OSError, IOError) as e:
            logging.error(f'"{output_file_basename}" convert FAILED!')
            raise
        except Exception as e:
            logging.error(f'"{output_file_basename}" convert FAILED!')
            if os.path.exists(output_file_path) and os.path.isfile(output_file_path):
                os.remove(output_file_path)
            raise

            return total_used_bytes

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

    @dataclass
    class MacroData:
        loop_pos: int = -1
        release_pos: int = -1
        data: List[int] = field(default_factory=list)

    def __convert_macro_data(self, data: List[Union[int, MacroItem]]) -> MacroData:
        result = self.MacroData()
        try:
            result.loop_pos = data.index(MacroItem.LOOP)
        except ValueError:
            pass

        try:
            result.release_pos = data.index(MacroItem.RELEASE)
        except ValueError:
            pass

        if result.loop_pos != -1 and result.release_pos != -1:
            if result.loop_pos > result.release_pos:
                result.loop_pos -= 1
            else:
                result.release_pos -= 1

        result.data = [x for x in data if type(x) == int]
        return result


if __name__ == "__main__":
    import argparse
    import sys

    logging.basicConfig(format="[%(levelname)s] %(message)s", level=logging.INFO)

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
        "--c-var-name", default=None, required=False, help="C variable identifier"
    )

    args = parser.parse_args()

    # Use `input_filename.c` as output file path
    if not args.output:
        args.output = os.path.splitext(args.input)[0] + ".c"
    # Use `output_filename` as C variable name
    if not args.c_var_name:
        args.c_var_name = os.path.splitext(os.path.basename(args.output))[0]

    fb_module = FurballModule(args.input)
    fb_module.write_file(args.output, args.c_var_name)
