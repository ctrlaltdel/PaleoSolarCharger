#!/usr/bin/env python

DIR = '/tmp/build5444690825086055604.tmp'

from pysimavr.avr import Avr, Firmware
avr=Avr(mcu='atmega48',f_cpu=8000000)
firmware = Firmware(DIR + '/PaleoSolarCharger.cpp.elf')
avr.load_firmware(firmware)
