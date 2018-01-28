# Makefile for Majek's Optiboot fork
# https://github.com/majekw/optiboot
#

# Edit History
# 201607xx: MCUdude: Rewrote the make routines and deleted all the extra makefiles.
#										 One can now simply run the makeall script to make all variants
#										 of this bootloader.	 
#											
# 201406xx: WestfW:  More Makefile restructuring.
#                    Split off Makefile.1284, Makefile.extras, Makefile.custom
#                    So that in theory, the main Makefile contains only the
#                    official platforms, and does not need to be modified to
#                    add "less supported" chips and boards.
#
# 201303xx: WestfW:  Major Makefile restructuring.
#                    Allows options on Make command line "make xx LED=B3"
#                    (see also pin_defs.h)
#                    Divide into "chip" targets and "board" targets.
#                    Most boards are (recursive) board targets with options.
#                    Move isp target to separate makefile (fixes m8 EFUSE)
#                    Some (many) targets will now be rebuilt when not
#                    strictly necessary, so that options will be included.
#                    (any "make" with options will always compile.)
#                    Set many variables with ?= so they can be overridden
#                    Use arduinoISP settings as default for ISP targets
#
#
# * Copyright 2013-2015 by Bill Westfield.  Part of Optiboot.
# * This software is licensed under version 2 of the Gnu Public Licence.
# * See optiboot_flash.c for details.

#----------------------------------------------------------------------
#
# program name should not be changed...
PROGRAM    = optiboot_flash

# The default behavior is to build using tools that are in the users
# current path variables, but we can also build using an installed
# Arduino user IDE setup, or the Arduino source tree.
# Uncomment this next lines to build within the arduino environment,
# using the arduino-included avrgcc toolset (mac and pc)
# ENV ?= arduino
# ENV ?= arduinodev
# OS ?= macosx
# OS ?= windows

# export symbols to recursive makes (for ISP)
export

# defaults
MCU_TARGET = atmega169
LDSECTIONS  = -Wl,--section-start=.text=0x3e00 -Wl,--section-start=.version=0x3ffe

# Build environments
# Start of some ugly makefile-isms to allow optiboot to be built
# in several different environments.  See the README.TXT file for
# details.

# default
fixpath = $(1)
SH := bash

ifeq ($(ENV), arduino)
# For Arduino, we assume that we're connected to the optiboot directory
# included with the arduino distribution, which means that the full set
# of avr-tools are "right up there" in standard places.
# (except that in 1.5.x, there's an additional level of "up")
TESTDIR := $(firstword $(wildcard ../../../tools/*))
ifeq (,$(TESTDIR))
# Arduino 1.5.x tool location compared to optiboot dir
  TOOLROOT = ../../../../tools
else
# Arduino 1.0 (and earlier) tool location
  TOOLROOT = ../../../tools
endif
GCCROOT = $(TOOLROOT)/avr/bin/

ifeq ($(OS), windows)
# On windows, SOME of the tool paths will need to have backslashes instead
# of forward slashes (because they use windows cmd.exe for execution instead
# of a unix/mingw shell?)  We also have to ensure that a consistent shell
# is used even if a unix shell is installed (ie as part of WINAVR)
fixpath = $(subst /,\,$1)
SHELL = cmd.exe
SH = sh
endif

else ifeq ($(ENV), arduinodev)
# Arduino IDE source code environment.  Use the unpacked compilers created
# by the build (you'll need to do "ant build" first.)
ifeq ($(OS), macosx)
TOOLROOT = ../../../../build/macosx/work/Arduino.app/Contents/Resources/Java/hardware/tools
endif
ifeq ($(OS), windows)
TOOLROOT = ../../../../build/windows/work/hardware/tools
endif

GCCROOT = $(TOOLROOT)/avr/bin/
AVRDUDE_CONF = -C$(TOOLROOT)/avr/etc/avrdude.conf

else
GCCROOT =
AVRDUDE_CONF =
endif

STK500 = "C:\Program Files\Atmel\AVR Tools\STK500\Stk500.exe"
STK500-1 = $(STK500) -e -d$(MCU_TARGET) -pf -vf -if$(PROGRAM)_$(TARGET).hex \
           -lFF -LFF -f$(HFUSE)$(LFUSE) -EF8 -ms -q -cUSB -I200kHz -s -wt
STK500-2 = $(STK500) -d$(MCU_TARGET) -ms -q -lCF -LCF -cUSB -I200kHz -s -wt
#
# End of build environment code.


OBJ        = $(PROGRAM).o
OPTIMIZE = -Os -fno-split-wide-types -mrelax -fno-caller-saves

DEFS       = 

#
# platforms support EEPROM and large bootloaders need the eeprom functions that
# are defined in libc, even though we explicity remove it with -nostdlib because
# of the space-savings.
LIBS       =  -lc

CC         = $(GCCROOT)avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -DF_CPU=$(AVR_FREQ) $(DEFS)
override LDFLAGS       = $(LDSECTIONS) -Wl,--relax -nostartfiles -nostdlib
#-Wl,--gc-sections

OBJCOPY        = $(GCCROOT)avr-objcopy
OBJDUMP        = $(call fixpath,$(GCCROOT)avr-objdump)

SIZE           = $(GCCROOT)avr-size

#
# Make command-line Options.
# Permit commands like "make atmega328 LED_START_FLASHES=10" to pass the
# appropriate parameters ("-DLED_START_FLASHES=10") to gcc
#

ifdef BAUD_RATE
BAUD_RATE_CMD = -DBAUD_RATE=$(BAUD_RATE)
dummy = FORCE
else
BAUD_RATE_CMD = -DBAUD_RATE=115200
endif

ifdef LED_START_FLASHES
LED_START_FLASHES_CMD = -DLED_START_FLASHES=$(LED_START_FLASHES)
dummy = FORCE
else
LED_START_FLASHES_CMD = -DLED_START_FLASHES=3
endif

# BIG_BOOT: Include extra features, up to 1K.
ifdef BIGBOOT
BIGBOOT_CMD = -DBIGBOOT=1
dummy = FORCE
endif

ifdef SOFT_UART
SOFT_UART_CMD = -DSOFT_UART=1
dummy = FORCE
endif

ifdef LED_DATA_FLASH
LED_DATA_FLASH_CMD = -DLED_DATA_FLASH=1
dummy = FORCE
endif

ifdef LED
LED_CMD = -DLED=$(LED)
dummy = FORCE
endif

ifdef SINGLESPEED
SS_CMD = -DSINGLESPEED=1
endif

COMMON_OPTIONS = $(BAUD_RATE_CMD) $(LED_START_FLASHES_CMD) $(BIGBOOT_CMD)
COMMON_OPTIONS += $(SOFT_UART_CMD) $(LED_DATA_FLASH_CMD) $(LED_CMD) $(SS_CMD)

#UART is handled separately and only passed for devices with more than one.
ifdef UART
UART_CMD = -DUART=$(UART)
endif

# Not supported yet
# ifdef SUPPORT_EEPROM
# SUPPORT_EEPROM_CMD = -DSUPPORT_EEPROM
# dummy = FORCE
# endif

# Not supported yet
# ifdef TIMEOUT_MS
# TIMEOUT_MS_CMD = -DTIMEOUT_MS=$(TIMEOUT_MS)
# dummy = FORCE
# endif
#

#.PRECIOUS: %.elf

#---------------------------------------------------------------------------
# "Chip-level Platform" targets.
# A "Chip-level Platform" compiles for a particular chip, but probably does
# not have "standard" values for things like clock speed, LED pin, etc.
# Makes for chip-level platforms should usually explicitly define their
# options like: "make atmega1285 AVR_FREQ=16000000L LED=D0"
#---------------------------------------------------------------------------
#
# Note about fuses:
# the efuse should really be 0xf8; since, however, only the lower
# three bits of that byte are used on the atmega168, avrdude gets
# confused if you specify 1's for the higher bits, see:
# http://tinker.it/now/2007/02/24/the-tale-of-avrdude-atmega168-and-extended-bits-fuses/
#
# similarly, the lock bits should be 0xff instead of 0x3f (to
# unlock the bootloader section) and 0xcf instead of 0x2f (to
# lock it), but since the high two bits of the lock byte are
# unused, avrdude would get confused.
#---------------------------------------------------------------------------
#

#ATmega169
atmega169: MCU_TARGET = atmega169
atmega169: CFLAGS += $(COMMON_OPTIONS)
atmega169: AVR_FREQ ?= 16000000L
atmega169: LDSECTIONS = -Wl,--section-start=.text=0x3e00
atmega169: atmega169/$(PROGRAM)_atmega169_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega169: atmega169/$(PROGRAM)_atmega169_$(BAUD_RATE)_$(AVR_FREQ).lst

#ATmega169p
atmega169p: MCU_TARGET = atmega169p
atmega169p: CFLAGS += $(COMMON_OPTIONS)
atmega169p: AVR_FREQ ?= 16000000L
atmega169p: LDSECTIONS = -Wl,--section-start=.text=0x3e00
atmega169p: atmega169p/$(PROGRAM)_atmega169p_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega169p: atmega169p/$(PROGRAM)_atmega169p_$(BAUD_RATE)_$(AVR_FREQ).lst

#ATmega329
atmega329: MCU_TARGET = atmega329
atmega329: CFLAGS += $(COMMON_OPTIONS)
atmega329: AVR_FREQ ?= 16000000L
atmega329: LDSECTIONS = -Wl,--section-start=.text=0x7e00
atmega329: atmega329/$(PROGRAM)_atmega329_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega329: atmega329/$(PROGRAM)_atmega329_$(BAUD_RATE)_$(AVR_FREQ).lst

#ATmega329p
atmega329p: MCU_TARGET = atmega329p
atmega329p: CFLAGS += $(COMMON_OPTIONS)
atmega329p: AVR_FREQ ?= 16000000L
atmega329p: LDSECTIONS = -Wl,--section-start=.text=0x7e00
atmega329p: atmega329p/$(PROGRAM)_atmega329p_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega329p: atmega329p/$(PROGRAM)_atmega329p_$(BAUD_RATE)_$(AVR_FREQ).lst

#ATmega3290
atmega3290: MCU_TARGET = atmega3290
atmega3290: CFLAGS += $(COMMON_OPTIONS)
atmega3290: AVR_FREQ ?= 16000000L
atmega3290: LDSECTIONS = -Wl,--section-start=.text=0x7e00
atmega3290: atmega3290/$(PROGRAM)_atmega3290_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega3290: atmega3290/$(PROGRAM)_atmega3290_$(BAUD_RATE)_$(AVR_FREQ).lst

#ATmega3290p
atmega3290p: MCU_TARGET = atmega3290p
atmega3290p: CFLAGS += $(COMMON_OPTIONS)
atmega3290p: AVR_FREQ ?= 16000000L
atmega3290p: LDSECTIONS = -Wl,--section-start=.text=0x7e00
atmega3290p: atmega3290p/$(PROGRAM)_atmega3290p_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega3290p: atmega3290p/$(PROGRAM)_atmega3290p_$(BAUD_RATE)_$(AVR_FREQ).lst

#ATmega649
atmega649: MCU_TARGET = atmega649
atmega649: CFLAGS += $(COMMON_OPTIONS) -DBIGBOOT $(UART_CMD)
atmega649: AVR_FREQ ?= 16000000L
atmega649: LDSECTIONS = -Wl,--section-start=.text=0xfc00 -Wl,--section-start=.version=0xfffe
atmega649: atmega649/$(PROGRAM)_atmega649_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega649: atmega640/$(PROGRAM)_atmega649_$(BAUD_RATE)_$(AVR_FREQ).lst

#ATmega649p
atmega649p: MCU_TARGET = atmega649p
atmega649p: CFLAGS += $(COMMON_OPTIONS) -DBIGBOOT $(UART_CMD)
atmega649p: AVR_FREQ ?= 16000000L
atmega649p: LDSECTIONS = -Wl,--section-start=.text=0xfc00 -Wl,--section-start=.version=0xfffe
atmega649p: atmega649p/$(PROGRAM)_atmega649p_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega649p: atmega649p/$(PROGRAM)_atmega649p_$(BAUD_RATE)_$(AVR_FREQ).lst

#ATmega6490
atmega6490: MCU_TARGET = atmega6490
atmega6490: CFLAGS += $(COMMON_OPTIONS) -DBIGBOOT $(UART_CMD)
atmega6490: AVR_FREQ ?= 16000000L
atmega6490: LDSECTIONS = -Wl,--section-start=.text=0xfc00 -Wl,--section-start=.version=0xfffe
atmega6490: atmega6490/$(PROGRAM)_atmega6490_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega6490: atmega6490/$(PROGRAM)_atmega6490_$(BAUD_RATE)_$(AVR_FREQ).lst

#ATmega6490p
atmega6490p: MCU_TARGET = atmega6490p
atmega6490p: CFLAGS += $(COMMON_OPTIONS) -DBIGBOOT $(UART_CMD)
atmega6490p: AVR_FREQ ?= 16000000L
atmega6490p: LDSECTIONS = -Wl,--section-start=.text=0xfc00 -Wl,--section-start=.version=0xfffe
atmega6490p: atmega6490p/$(PROGRAM)_atmega6490p_$(BAUD_RATE)_$(AVR_FREQ).hex
atmega6490p: atmega6490p/$(PROGRAM)_atmega6490p_$(BAUD_RATE)_$(AVR_FREQ).lst



#---------------------------------------------------------------------------
#
# Generic build instructions
#

FORCE:

baudcheck: FORCE
	- @$(CC) --version
	- @$(CC) $(CFLAGS) -E baudcheck.c -o baudcheck.tmp.sh
	- @$(SH) baudcheck.tmp.sh

isp: $(TARGET)
	$(MAKE) -f Makefile.isp isp TARGET=$(TARGET)

isp-stk500: $(PROGRAM)_$(TARGET).hex
	$(STK500-1)
	$(STK500-2)

%.elf: $(OBJ) baudcheck $(dummy)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)
	$(SIZE) $@

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.srec *.bin *.hex *.tmp.sh
	rm -rf atmega169/*.hex atmega169/*.lst
	rm -rf atmega169p/*.hex atmega169p/*.lst
	rm -rf atmega329/*.hex atmega329/*.lst
	rm -rf atmega329p/*.hex atmega329p/*.lst
	rm -rf atmega3290/*.hex atmega3290/*.lst
	rm -rf atmega3290p/*.hex atmega3290p/*.lst
	rm -rf atmega649/*.hex atmega649/*.lst
	rm -rf atmega649p/*.hex atmega649p/*.lst
	rm -rf atmega6490/*.hex atmega6490/*.lst
	rm -rf atmega6490p/*.hex atmega6490p/*.lst
	rm -rf baudcheck.tmp.sh

clean_asm:
	rm -rf atmega169/*.lst
	rm -rf atmega169p/*.lst
	rm -rf atmega329/*.lst
	rm -rf atmega329p/*.lst
	rm -rf atmega3290/*.lst
	rm -rf atmega3290p/*.lst
	rm -rf atmega649/*.lst
	rm -rf atmega649p/*.lst
	rm -rf atmega6490/*.lst
	rm -rf atmega6490p/*.lst
	rm -rf baudcheck.tmp.sh

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -j .version --set-section-flags .version=alloc,load -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -j .version --set-section-flags .version=alloc,load -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -j .version --set-section-flags .version=alloc,load -O binary $< $@
