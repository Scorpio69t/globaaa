<?php
	for ($i = 9000; $i < 10000; $i++) {

	  $file1 = (string) $i;
	  $file2 = (string) $i + 1;

	  // add leading zeroes to files
          while (strlen($file1) < 4) {
            $file1 = '0' . $file1;
          }

          while (strlen($file2) < 4) {
            $file2 = '0' . $file2;
          }

	  $file1 = "file_" . $file1 . ".txt";
	  $file2 = "file_" . $file2 . ".txt";
	  $cmd = "diff " . $file1 . " " . $file2;

	  // check that the two files are identical
	  $output = shell_exec($cmd);

	  if ($output != "") {
	    echo $file2;
	    exit();
          }
	}

	echo "files all equal";
?>
