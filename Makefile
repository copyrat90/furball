ifeq ($(and $(strip $(DEVKITPRO)),$(strip $(DEVKITARM))),)
$(error Make sure DEVKITPRO and DEVKITARM are correctly set in your environment.)
endif

PROJECT		:= furball

BUILD_LIB	:= yes

#
# Furball Makefile options
#
# see `README.md` for details.
#

# FB_MGBA_LOG_ENABLED			:= 1
# FB_MGBA_LOG_MAY_FATAL_CRASH	:= 1


#
# Files
#
# All options support glob patterns like `src/*.c`.
#

# Binary files to process with bin2s
BINARY_FILES	:=

# Graphics files to process with grit
#
# Every file requires an accompanying .grit file,
# so gfx/test.png needs gfx/test.grit
GRAPHICS	:=

# Source files to compile
SOURCES		:= src/*.c

# Include directories
INCLUDES	:= include include_private

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
		$(if $(strip $(FB_MGBA_LOG_ENABLED)),-DFB_MGBA_LOG_ENABLED,) \
		$(if $(strip $(FB_MGBA_LOG_MAY_FATAL_CRASH)),-DFB_MGBA_LOG_MAY_FATAL_CRASH,) \

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
