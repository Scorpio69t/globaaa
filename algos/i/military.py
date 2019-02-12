#!/usr/bin/python

def MilToStandard(s):
  # separate string at delimiter
  [hour, remainder] = s.split(":")

  ending = ""
  if int(hour) / 12 == 1:
    ending = "pm"
  else:
    ending = "am"

  if ((int(hour) % 12) == 0):
    return ("12:" + remainder + ending)
  else:
    return (str((int(hour) % 12)) + ":" + remainder + ending)

def main():
  assert(MilToStandard("14:54.10") == "2:54.10pm")
  assert(MilToStandard("20:01.19") == "8:01.19pm")
  assert(MilToStandard("00:59.59") == "12:59.59am")
  assert(MilToStandard("12:00.00") == "12:00.00pm")
  assert(MilToStandard("8:10.04") == "8:10.04am")
  assert(MilToStandard("23:59.59") == "11:59.59pm")

if __name__=='__main__':
  main()
