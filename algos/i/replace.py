#!/usr/bin/python

def replace(s, w):
  return s.replace("hello", w)

def main():
  s = ("In this sentence, I want to replace all instances of 'hello' with 'hihi', so when I'm done there should be no more instances of 'hello'")

  print replace(s, "hihi")

if __name__=='__main__':
  main()
