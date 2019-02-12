import sys
import socket
import time
import binascii

# taken from cs162 warmup
class Client:
  def __init__(self, host, port):
    self.host = host
    self.port = port
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

  def close(self):
    self.sock.close()

  def connect(self):
    try:
      self.sock.connect((self.host, self.port))
    except Exception as e:
      print "[-] Connect failed. ({})".format(e)

  def send(self, msg):
    msglen = len(msg)
    sent = self.sock.send(msg)
    if sent == 0 or sent != msglen:
      raise RuntimeError("error sending message")

  def recv(self):
    msg = []    
    msg = self.sock.recv(256)
    return msg  

def send_msg(client, iv, cipher):
  client.send(iv + cipher + "\n")
  time.sleep(.005)
  msg = client.recv()
  print msg

# create the IV (from the already discovered partial intermediate state) that'll
# return the necessary padding (size + 1)  
def create_base(d, size):
  s = ""

  if size == 0:
    return s
  else:
    for i in range(0, size):
      s = find_xor(size + 1, d[str(i + 1)]) + s
    return s

# called in create_base(); finds 'x' in "x ^ num = base' | num is a number in the
# intermediate state and base is usually padding [1, 16]
def find_xor(base, num):
  for i in range(0, 256):
    if num ^ i == base:
      break    

  h = hex(i)[2:]
  if len(h) < 2:
    return "0" + h
  else:
    return h

# takes a msg and returns intermediate state
def try_ivs(client, cipher):
  bdict = {}  
  c2 = cipher

  # pad zeros
  l = 32 - len(c2)
  while l != 0:
    c2 = c2 + "0"
    l = l - 1

  # use padding response to determine intermediate state
  for byte in range(1, 17):
    i = 0
    base = create_base(bdict, byte - 1)

    while i <= 0xff:
      iv = hex(i)[2:]
      iv = iv + base
  
      l = 32 - len(iv)
      while l != 0:
        iv = "0" + iv
        l = l - 1

      client.send(iv + c2 + "\n")
      time.sleep(.005)
      msg = client.recv()

      # means the padding was correct
      error = 'invalid or unrecognized command; try typing "help"\nSend an encrypted command:\n'
      error1 = "no command given\nSend an encrypted command:\n"
      if msg == error or msg == error1:
        bdict[str(byte)] = int("0x" + find_xor(byte, i), 0)
	break

      i = i + 1

  return bdict

# return IV s.t. given IS in d IV ^ IS = w
def find_word(d, w):
  # unpack word w
  whex = binascii.hexlify(w)

  # pad word
  pad = 16 - len(w)
  l = pad
  while l >= 0:
    whex = whex + "0" + hex(pad)[2:]
    l = l - 1

  # create IV
  base_hex = ""
  for i in range(1, 17):
    shex = hex(d[str(i)])[2:]
    if len(shex) == 1:
      shex = "0" + shex
    base_hex = shex + base_hex

  # find the correct IV s.t. when XOR'd with base_hex will produce whex
  iv = ""
  for i in range(0, 16):
    base_slice = base_hex[2*i:2*i+2]
    w_slice = whex[2*i:2*i+2]
    for k in range(0, 256):
      tmphex = ""
      if k ^ int("0x" + base_slice, 0) == int("0x" + w_slice, 0):
        tmphex = hex(k)[2:]
        if len(tmphex) == 1:
          tmphex = "0" + tmphex
        iv = iv + tmphex
        break

  return iv

def main():

  if len(sys.argv) != 4: 
    print "<server> <host> <port> <cmd>"
    return 

  # check cmd line args
  host = sys.argv[1]
  port = int(sys.argv[2])
  cmd = sys.argv[3] 

  # create an array of cmd line arguments
  i = 0
  cmdarray = []
  cmdlen = len(cmd)
  while i < cmdlen:
    cmdarray.append(cmd[i:i+16])
    i = i + 16

  # initial connection to server
  client = Client(host, port)
  client.connect()
  msg = client.recv()
  print msg

  # initial cipher
  cipher = '00000000000000000000000000000000'
  iv = ''
  totalcipher = cipher
  i = len(cmdarray) - 1

  # turn cmd plaintext into encrypted input 
  while i >= 0:    
    dict = try_ivs(client, cipher)
    iv = find_word(dict, cmdarray[i])
    i = i - 1
    if i < 0:
      break
    totalcipher = iv + totalcipher
    cipher = iv

  send_msg(client, iv, totalcipher)
  client.close()
  return

main()
