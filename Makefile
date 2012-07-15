ARDUINO_DIR  = /usr/share/arduino
AVR_TOOLS_PATH = /usr/bin
PARSE_BOARD = ../Arduino-Makefile/arduino-mk/ard-parse-boards

TARGET       = CLItest
ARDUINO_LIBS = LiquidCrystal

BOARD_TAG    = uno
ARDUINO_PORT = /dev/cu.usb*

include ../Arduino-Makefile/arduino-mk/Arduino.mk
