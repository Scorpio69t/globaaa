#!/usr/bin/python
import sys
import random
import socket
import numpy as np

# 2^24 - 1
maxGuess = 16777215

# define max length for server response
maxLen = 256

# padding
def padding(s, limit):
  while limit > 0:
    s = "0" + s
    limit = limit -1 
  return s

class Cipher(object):
	'''
	General description of a Cipher.
	'''
	def encrypt(self, data):
	  pass
	def decrypt(self, ctext):
	  pass

class XTEACipher(Cipher):
	'''
	The XTEA Cipher. (https://en.wikipedia.org/wiki/XTEA#Implementations)
	'''
	def __init__(self, key):
		self.key = key
		self.numRounds = 64
		self.delta = 0x9E3779B9

	def encrypt(self, data):

	  sum = 0

	  # data converted into string of hex digits
	  hexString = ""

	  # pad if necessary
          pad = 8 - len(data)
	  data = padding(data, pad)

	  for l in range(0, len(data)):
	    hexString += str(hex(ord(data[l]))[2:])

	  # break up data (smaller number goes into higher index)
	  v0 = int("0x" + hexString[0:8], 0)
	  v1 = int("0x" + hexString[8:], 0)

	  # key as a string
	  hexKey = str(self.key)

	  # break up key
	  k0 = int("0x" + hexKey[0:8], 0)
	  k1 = int("0x" + hexKey[8:16], 0)
	  k2 = int("0x" + hexKey[16:24], 0)
	  k3 = int("0x" + hexKey[24:32], 0)
	  keyArray = [k0, k1, k2, k3]

	  # algorithm; ensure overflow similar to C. This will cause warnings; run with -W ignore
	  for i in range(0, self.numRounds):
	    	v0left = np.uint32(np.uint32(np.uint32(v1 << 4) ^ np.uint32(v1 >> 5)) + v1)
	  	v0right = np.uint32(sum + keyArray[sum & 3])
	    	v0 = np.uint32(v0 + np.uint32(v0left ^ v0right))
	    	sum = np.uint32(sum + self.delta)
	    	v1 = np.uint32(v1 + np.uint32(np.uint32(np.uint32(np.uint32(v0 << 4) ^ np.uint32(v0 >> 5)) + v0) ^ np.uint32(sum + keyArray[(sum >> 11) & 3])))	    

	  v0 = str(hex(v0))[2:-1]

	  # pad if necessary
	  pad = 8 - len(v0)
	  v0 = padding(v0, pad)

	  v1 = str(hex(v1))[2:-1]

	  # pad if necessary
          pad = 8 - len(v1)
	  v1 = padding(v1, pad)

	  return v0 + v1


	def decrypt(self, data):

	  sum = self.delta * self.numRounds

	  # data converted into string of hex digits
	  hexString = ""

	  # remove newline
	  data = data[:-1]

	  # pad if necessary
          pad = 16 - len(data)
	  data = padding(data, pad)

	  # break up data (smaller number goes into higher index)
	  v0 = int("0x" + str(data)[0:8], 0)
	  v1 = int("0x" + str(data)[8:], 0)

	  # key as a string
	  hexKey = str(self.key)

	  # break up key
	  k0 = int("0x" + hexKey[0:8], 0)
	  k1 = int("0x" + hexKey[8:16], 0)
	  k2 = int("0x" + hexKey[16:24], 0)
	  k3 = int("0x" + hexKey[24:32], 0)
	  keyArray = [k0, k1, k2, k3]

	  # algorithm; ensure overflow similar to C. This will cause warnings; run with -W ignore
	  for i in range(0, self.numRounds):
	    	v1 = np.uint32(v1 - np.uint32(np.uint32(np.uint32(np.uint32(v0 << 4) ^ np.uint32(v0 >> 5)) + v0) ^ np.uint32(sum + keyArray[(sum >> 11) & 3])))	    
	    	sum = np.uint32(sum - self.delta)
	    	v0left = np.uint32(np.uint32(np.uint32(v1 << 4) ^ np.uint32(v1 >> 5)) + v1)
	    	v0right = np.uint32(sum + keyArray[sum & 3])
	    	v0 = np.uint32(v0 - np.uint32(v0left ^ v0right))

	  return (str(hex(v0))[2:-1] + str(hex(v1))[2:-1]).decode("hex")

class NOPCipher(Cipher):
	'''
	A cipher that does nothing. If there's no key specified, the code can just use this.
	'''
	def encrypt(self, data):
	  return data

	def decrypt(self, ctext):
	  return ctext

class Game:
    # Converted from python code @ https://docs.python.org/2/howto/sockets.html.
    def __init__(self, host, port, key=None):
    	self.host = host
    	self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    	if key:
    	    # Encryption is enabled.
    	    self.cipher = XTEACipher(key)
    	else:
      	    # Encryption does nothing.
    	    self.cipher = NOPCipher()

    def close(self):
    	'''
    	Close the socket.
    	'''
    	self.sock.close()

    def connect(self):
    	try:
        	self.sock.connect((self.host, self.port))
        except Exception as e:
        	print "[-] Connect failed. ({})".format(e)


    def send(self, msg):
    	'''
    	Sends msg to the server, after running it through the cipher.
    	'''
    	ctext = self.cipher.encrypt(msg) + "\n"
	msglen = len(ctext)
        sent = self.sock.send(ctext)

	if sent == 0 or sent != msglen:
          raise RuntimeError("error sending message")


    def recv(self):
    	'''
    	Receives data from the server, and performs decryption.
    	'''
	msg = []
    	msg = self.sock.recv(maxLen)
        return self.cipher.decrypt(msg)

# log(n)
def binarySearch(min, max, guess, game):
	# interact with server
    	game.send(str(guess))
    	resp = game.recv()

    	# check output from server
    	if resp == "Correct!" or resp == "Correct!\n":
      		return guess
    	elif resp == "Up...\0\0\0" or resp == "Up...\n":
      		return binarySearch(guess, max, (max + guess) / 2, game)
    	else:
      		return binarySearch(min, guess, (min + guess) / 2, game)

def runGame(host, port, key=None):
	game = Game(host, port, key=key)
    	game.connect()
    	print binarySearch(0, maxGuess, maxGuess / 2, game)
    	game.close()
    	return

def main():
    	if len(sys.argv) < 3:
      		print "Usage: ./sol.py <host> <port> [<key>]"
      		print "host: The host to connect to (where ./guess is running)."
      		print "port: The port to connect to."
      		print "key (optional): The key to use for encryption/decryption."
      		return
    	host = sys.argv[1]
    	port = int(sys.argv[2])
    	key = sys.argv[3] if len(sys.argv) > 3 else None
    	runGame(host, port, key=key)

########
# Main
########
main()
########
# I <3 Computers
########
