#!/bin/bash

# make a dir temp
mkdir 'temp'

# little loop to give race.sh time to run
for i in `seq 1 1000`; do
        :
done

# if you cant cd (cause the dir might've been deleted) create a
# new directory foo. This idea here, then, is race should interrupt
# this process after it makes temp, will delete temp, and this wil
# cause this process to create foo.
if ! [ -d /home/dfarnha1/course/cs166/6/notes/race/temp ]
  then
	mkdir 'foo'
fi

