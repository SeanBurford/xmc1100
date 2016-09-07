#!/usr/bin/python

import sys
import time

Vref=3.0

def DisplayVoltage(channel, reading):
  voltage = ((float(reading) / 4096) * Vref) * (1 / (5.6 / 125.6))
  print 'channel %d %03X %5.2f volts' % (channel, reading, voltage)


def DisplayCurrent(channel, reading):
  current = (((float(reading) / 4096) * Vref) / 20) / 0.005
  print 'channel %d %03X %5.2f amps' % (channel, reading, current)


def DisplayTemperature(channel, reading):
  temperature = (((float(reading) / 4096) * Vref) - 0.5) / 0.01
  print 'channel %d %03X %5.2f celsius' % (channel, reading, temperature)


def ReadSerial():
  with open('/tmp/powerlog.txt', 'w') as log:
    with open('/dev/ttyACM0', 'r') as fd:
      line = ''
      while True:
        line += fd.read(1)
        if line[-1] == '\n':
          log.write('%d %s' % (time.time(), line))
          log.flush()
          for word in line.split():
            val = int(word, 16)
            if val == 0:
              continue
            channel = val >> 12
            reading = val & 0x0FFF
            if channel in (0, 2):
              DisplayVoltage(channel, reading)
            elif channel in (1, 3):
              DisplayCurrent(channel, reading)
            elif channel in (4,):
              DisplayTemperature(channel, reading)
          print ''
          line = ''


def main():
  ReadSerial()


if __name__ == '__main__':
  main()
