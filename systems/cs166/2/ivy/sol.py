#!/usr/bin/python

import pexpect
import re
import time 

#
# Key: 0x9544cd8a1bb10525
#

def comm_server():
  # open a connection to server
  child = pexpect.spawn("/course/cs1660/student/dfarnha1/cryptography/ivy/ivy")

  # find ciphertext and iv of sent key
  match = re.search(r'(\w+) (\w+)', child.readline())
  iv = match.group(1)
  key = match.group(2)

  # bit mask to get random # r
  bitmask = "0000000000000000"

  siv = ""
  r = ""

  # keep giving input to server until we happen to generate the same iv
  while siv != iv:
    # send bitmask to server; skip first return line
    child.sendline(bitmask)
    child.readline()
    match = re.search(r'(\w+) (\w+)', child.readline())
    siv = match.group(1)
    r = match.group(2)

  return (int("0x" + key, 0) ^ int("0x" + r, 0))

def main():
  print comm_server()

main()
