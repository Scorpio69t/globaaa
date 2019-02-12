#!/bin/bash

#
# The goal would be to run the cs666_handin in a background process,
# pause it, replace whatever contents it places in the /tmp folder
# with a new file that'll then run in the autograder
#
#	the key here would be to use:
#
#	  &: run in the background
#	  $!: the PID of the process running in the background.
#

# start running makedir script in background
./makedir.sh &

# now, the below will run after makedir is finished. If you take this
# out you'll get dir foo
#for i in `seq 1 5000`; do
#        :
#done

while [ 1 ]
do
  # if you switched into temp
  if [ -d /home/dfarnha1/course/cs166/6/notes/race/temp ]
    then
      # stop the process makedir and delete temp
#      kill -TSTP $!
#      cd ..
      rm -rf temp
#      kill -CONT $!
      break
  fi
done
