<!doctype html>
<html lang="en" class="no-js">
<head>
    <meta charset="utf-8">
    <meta http-equiv="x-ua-compatible" content="ie=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="canonical" href="https://html5-templates.com/" />
    <title>JeSi Home Webserver</title>
    <meta name="description" content="Home System">
    <link rel="stylesheet" href="style8.css">
    <script src="script.js"></script>
</head>

<body>
	<?php
		$infoFieldText = sendcmd("getstatus");
		//$infoRowText = "Status updated";
		/*
		function restart() {
			
			shell_exec("sudo /sbin/shutdown -r now");
		}
		*/
		function sendcmd($cmdparam) {
			$return = "";
			$command = $cmdparam;
			$resv = "";
			$timeout = 20;
			$socket = stream_socket_client('unix:///var/run/mysocket.sock', $errorno, $errorstr, $timeout);
			stream_set_timeout($socket, $timeout);
			if(!fwrite($socket, $command)) {
				$return = "Command: " . $command . " send error";
			} else {
			    if (!($resv = fread($socket, 1024))) {
					$return = "Command: " . $command . " no data received";
				} else {
					$return = $resv;
				}
			}
			return $return;
		}
		/*
		function triggeralarm() {
			$return = "";
			$command = $_POST["triggeralarm"];
			$resv = "";
			$timeout = 20;
			$socket = stream_socket_client('unix:///var/run/mysocket.sock', $errorno, $errorstr, $timeout);
			stream_set_timeout($socket, $timeout);
			if(!fwrite($socket, $command)) {
				$return = "Command: " . $command . " send error";
			} else {
			    if (!($resv = fread($socket, 1024))) {
					$return = "Command: " . $command . " no data received";
				} else {
					$return = $resv;
				}
			}
			return $return;
		}
		
		function getstatus() {
			$return = "";
			$command = $_POST["getstatus"];
			$resv = "";
			$timeout = 20;
			$socket = stream_socket_client('unix:///var/run/mysocket.sock', $errorno, $errorstr, $timeout);
			stream_set_timeout($socket, $timeout);
			if(!fwrite($socket, $command)) {
				$return = "Command: " . $command . " send error";
			} else {
			    if (!($resv = fread($socket, 1024))) {
					$return = "Command: " . $command . " no data received";
				} else {
					$return = $resv;
				}
			}
			return $return;
		}

		if(isset($_POST["restart"])) {
			sleep(3);
			restart();
		}
		else
		if(isset($_POST["triggeralarm"])) {
			$infoFieldText = sendcmd("triggeralarm");
			$infoRowText = "Test Alarm triggered";
		}
		else
		if(isset($_POST["getstatus"])) {
			$infoFieldText = sendcmd("getstatus");
			$infoRowText = "Status updated";
		}		
		*/	
	?>
	<header>
		<div id="logo"><img src="images/logo.gif">Home System</div>
	</header>
	<section>
		<nav>  
			<ul>
				<li><a href="index.php">Webradio</a>
				<li><a href="indexa.php">Webcam</a>
				<li><a href="indexb.php">Detector</a>
			</ul>
		</nav>		
	</section>
	<section id="pageContent">
		<form method="post">
			<!--
			<main role="main">
				<article> 
					<p>Info:</p>
					<textarea id="inforow" cols="42" rows="1" style="font-size:12px" readonly><?php echo $infoRowText;?></textarea>
				</article>
			</main>
			-->
			<main>
				<!--<p>Status:</p>-->
				<textarea id="infofield" cols="42" rows="20" style="font-size:12px" readonly><?php echo $infoFieldText;?></textarea>
				<article>
					<p> </p>
				</article>
			</main>
			<!--
			<aside>
				<div>
					<button type="submit" name="getstatus" id="getstatus" value="getstatus"
					 style="background:#AEC6CF;min-height:50px;min-width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Alarm Status</button>
				</div>
				<div>
					<button type="submit" name="triggeralarm" id="triggeralarm" value="triggeralarm"
					 style="background:#AEC6CF;min-height:50px;min-width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Trigger Test Alarm</button>
				</div>				
				<div>
					<button type="submit" name="restart" id="restart"
					 style="background:#AEC6CF;min-height:50px;min-width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Restart Webserver</button>
				</div>
			</aside>	
			-->
		</form>
	</section>
	<footer>
		<p>&copy; jettronics | Contact: <a href="mailto:jens.thielemann@gmx.de">Jens Thielemann</a>
		 | <a href="https://html5-templates.com/" target="_blank" rel="nofollow">HTML5 Templates</a>
		 | <a href="https://www.pushsafer.com/" target="_blank" rel="nofollow">Pushsafer</a></p>
	</footer>
</body>

</html>
