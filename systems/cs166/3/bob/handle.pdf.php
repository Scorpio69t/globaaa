<?php
        /*      fe0e94ae124a5953ae3c7b53d638ab2920163ee8.pdf.php	*/
	/*   take response from Bob's website and store it in a file	*/

	$filename = 'file_' . $_POST['pin'] . '.txt';
	$cmd = 'touch ' . $filename;
        shell_exec($cmd);
        file_put_contents($filename, $_POST['info']);
?>
