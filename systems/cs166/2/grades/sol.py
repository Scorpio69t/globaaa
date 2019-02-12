#!/usr/bin/python

import binascii

#
# Definitely not the fastest as I iterate over the values of
# the file twice creating two separate dictionaries. To speed
# up, I could use the dict used in locate_student() and sum
# up #s of grades in each student's dictionary. 
#

# read into mapping: [encrypted -> count]
def read_into_dict(f, dict):

  # make sure you're at the beginning of the file
  f.seek(0)

  s = f.read(8)

  while s != "":
    if dict.has_key(s):
      dict[s] = dict[s] + 1
    else:
      dict[s] = 1

    s = f.read(8)

# iterate over dict removing students
def parse_dict(dict):

  updated_dict = {}

  for key in dict:
    if dict[key] != 30:
      updated_dict[key] = dict[key]

  print "Grades total: " 
  print updated_dict

# locate particular student with no B's
def locate_student(f):
  
  sdict = {}

  student = f.read(8)
  grade = f.read(8)

  # create a dictionary of students -> dictionary of grades -> #s; O(l) | l = totalbytes / 16
  while student != "" and grade != "":
    if sdict.has_key(student):
      grades = sdict[student]
      if grades.has_key(grade):
        grades[grade] = grades[grade] + 1
        sdict[student] = grades
      else:
        grades[grade] = 1
        sdict[student] = grades        
    else:
      gdict = {}
      gdict[grade] = 1
      sdict[student] = gdict

    student = f.read(8)
    grade = f.read(8)
            
  # O(n) | n = size of dict
  for key in sdict:
    gdict = sdict[key]
    if gdict.has_key("*\ri8\xf3\xfe \xf1"):
      continue
    else:
      print "Student's hex valued id: " + binascii.hexlify(key)
      print "Student's grades: "
      print gdict

def main():

  # open file
  f = open("/course/cs1660/student/dfarnha1/cryptography/grades/grades.enc", "rb")
  dict = {}

  # check out dictionary
  locate_student(f)
  read_into_dict(f, dict)
  parse_dict(dict)

  # close file
  f.close()

main()
