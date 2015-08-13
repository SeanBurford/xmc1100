#!/usr/bin/python
"""Calculate usic parameters for 9600 and 115200 baud.

THere was a bit of related discussion here:
http://www.infineonforums.com/threads/2368-Defines-for-common-baud-rate
"""

def SolveForLinear(fsys):
  for restart_val in range(1, 0x400):
    clock_every = 0x400 - restart_val
    clock_rate = fsys / clock_every
    for pctq in (1, 2, 3, 4, 8):  # Length of a time quanta (in fPDIV)
      for dctq in (15, 13, 11, 9):  # Number of time quanta in a bit
        bit_rate = clock_rate * (1/(pctq + 1.0)) * (1/(dctq + 1.0))
        div_9600 = (bit_rate / 9600) - 1
        if div_9600 >= 1024:
          continue
        div_115200 = (bit_rate / 115200) - 1
        if div_115200 <= 1:
          continue
        assert div_9600 > div_115200
        actual_9600 = bit_rate / (int(div_9600) + 1)
        actual_115200 = bit_rate / (int(div_115200) + 1)
        print 'reset=%03X PCTQ=%d DCTQ=%d (%d-%d) %X %f %X %f' % (
            restart_val, pctq, dctq, bit_rate/1024, bit_rate, div_9600,
            actual_9600, div_115200, actual_115200)


def SolveForFractional(fsys):
  for pctq in (1, 2, 3, 4):  # Length of a time quanta (in fPDIV)
    for dctq in (15, 9):  # Number of time quanta in a bit
      for step in range(0x400):
        bit_rate = fsys * (step/1024.0) * (1/(pctq + 1.0)) * (1/(dctq + 1.0))
        div_9600 = (bit_rate / 9600) - 1
        if div_9600 >= 1024:
          continue
        div_115200 = (bit_rate / 115200) - 1
        if div_115200 <= 1:
          continue
        assert div_9600 > div_115200
        actual_9600 = bit_rate / (int(div_9600) + 1)
        actual_115200 = bit_rate / (int(div_115200) + 1)
        if int(actual_115200) <= 115250:
          print 'step=%03X PCTQ=%d DCTQ=%d (%d-%d) %X %f %X %f' % (
              step, pctq, dctq, bit_rate/1024, bit_rate, div_9600, actual_9600,
              div_115200, actual_115200)

def main():
  fsys = 32000000.0
  SolveForFractional(fsys)
  SolveForLinear(fsys)

if __name__ == '__main__':
  main()

