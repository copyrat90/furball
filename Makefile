ifeq ($(and $(strip $(DEVKITPRO)),$(strip $(DEVKITARM))),)
$(error Make sure DEVKITPRO and DEVKITARM are correctly set in your environment.)
endif

# Name of your ROM
#
# Add _mb to the end to build a multiboot ROM.
PROJECT		:= furball

# Uncomment this if you're building a library
#
BUILD_LIB	:= yes

# Options for gbafix (optional)
#
# Title:        12 characters
# Game code:     4 characters
# Maker code:    2 characters
# Version:       1 character
ROM_TITLE	:=
ROM_GAMECODE	:=
ROM_MAKERCODE	:=
ROM_VERSION	:=

#
# Files
#
# All options support glob patterns like `src/*.c`.
#

# Binary files to process with bin2s
BINARY_FILES	:= src/hello.bin

# Graphics files to process with grit
#
# Every file requires an accompanying .grit file,
# so gfx/test.png needs gfx/test.grit
GRAPHICS	:=

# Source files to compile
SOURCES		:= src/*.c
SOURCES		+= $(if $(strip $(FB_MGBA_LOG)),pvt/src/mgba.c,)

# Include directories
INCLUDES	:= include pvt/include

#
# Dependencies
#

# Library directories, with /include and /lib
LIBDIRS		:=

# Libraries to link
LIBS		:=

#
# Directories
#

# All build output goes here, except for built `*.a`
BUILDDIR	:= build

# Built `*.a` goes here
BUILDLIBDIR	:= lib

#
# Build Options
#

# Compiler flags (all languages)
WARNINGS	:= -Wall -Wextra -Wpedantic -Wshadow -Wundef -Wunused-parameter -Wmisleading-indentation \
		-Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wswitch-default
ALLFLAGS	:= $(WARNINGS) -g3 -gdwarf-4 -O2 \
		-ffunction-sections -fdata-sections \
		-masm-syntax-unified \
		-D_DEFAULT_SOURCE \
		$(if $(strip $(FB_MGBA_LOG)),-DFB_MGBA_LOG,)

# C compiler flags
CFLAGS		:= -std=c11

# C++ compiler flags
CXXFLAGS	:= -std=c++20 -fno-rtti -fno-exceptions

# Assembler flags (as passed to GCC)
ASFLAGS		:=

# Linker flags (as passed to GCC)
LDFLAGS		:= -mthumb \
		$(if $(filter %_mb,$(PROJECT)),-specs=gba_mb.specs,-specs=gba.specs)

# Uncomment this if you want to use Link Time Optimization
#
# USE_LTO		:= yes

include build.mk
