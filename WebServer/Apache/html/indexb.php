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
    <style type="text/css">  
		#boxSource { 
			position: relative;   
			height: 350px; width: 180px;    
			left: 0px; top: 0px; }    
		#boxControl { 
			position: relative;   
			height: 350px; width: 180px;    
			left: 180px; top: -350px; }     
		#boxWeather { 
			position: relative;   
			height: 350px; width: 330px;    
			left: 380px; top: -700px; }  	
		#boxArticle { 
			position: relative;  
			left: 0px; top: -690px; }  
		#boxFooter { 
			position: relative;  
			left: 0px; top: -700px; }  		      
	</style>     
    <!--<script src="script.js"></script>-->
</head>

<body>
	<?php

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
		
		if(isset($_POST["restart"])) {
			shell_exec("sudo /sbin/shutdown -r now");
		}
		else
		{
			if( empty($_POST) == false ) {
				foreach($_POST as $k => $v) { 
					$alarmStatus = sendcmd($v);
				}
			} else {
				$alarmStatus = sendcmd("getalarmstatus");
			}
		}
		
		$infoFieldText = sendcmd("getstatus");
		
		if( stripos($alarmStatus, "1: 0") !== false ) {
			$colorBathroomWater = "66ce64";
		} else {
			$colorBathroomWater = "df9696";
		}		
		
		if( stripos($alarmStatus, "2: 0") !== false ) {
			$colorKitchenWater = "66ce64";
		} else {
			$colorKitchenWater = "df9696";
		}	
		
		if( stripos($alarmStatus, "3: 0") !== false ) {
			$colorToiletWater = "66ce64";
		} else {
			$colorToiletWater = "df9696";
		}	
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
				<div id="boxSource" style="text-align:center">
					<button type="button" name="bathroomwater" id="bathroomwater" value="bathroomwater"
					 style="background:#<?php echo $colorBathroomWater; ?>;min-height:50px;min-width:150px;font-size:15px;font-weight:bold;
					 color:#444;border-radius:10px;">Bathroom Water</button>
					<button type="button" name="kitchenwater" id="kitchenwater" value="kitchenwater"
					 style="background:#<?php echo $colorKitchenWater; ?>;min-height:50px;min-width:150px;font-size:15px;font-weight:bold;
					 color:#444;border-radius:10px;">Kitchen Water</button>
					<button type="button" name="toiletwater" id="toiletwater" value="toiletwater"
					 style="background:#<?php echo $colorToiletWater; ?>;min-height:50px;min-width:150px;font-size:15px;font-weight:bold;
					 color:#444;border-radius:10px;">Guest Toilet Water</button>
				</div>
				<div id="boxControl" style="text-align:center">
					<form method="post">
					<button type="submit" name="resetalarm" id="resetalarm" value="resetalarm"
					 style="background:#BEC6CF;min-height:50px;min-width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Reset Alarm</button>
					<button type="submit" name="getalarmstatus" id="getalarmstatus" value="getalarmstatus"
					 style="background:#BEC6CF;min-height:50px;min-width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Alarm Status</button>
					<button type="submit" name="restart" id="restart"
					 style="background:#BEC6CF;min-height:50px;min-width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Restart Webserver</button>
					</form>
				</div>	
				<!--
				<div id="boxStream" style="text-align:center">
					<textarea id="infofield" cols="42" rows="20" style="font-size:12px" readonly><?php echo $infoFieldText;?></textarea>	
				</div>
				-->
				<div id="boxWeather" style="text-align:center">
					<div id="wcom-6ead28b0c2a8caade1dc54dbc8e72293" class="wcom-default w300x250" style="border: 1px solid rgb(204, 204, 204); background-color: 
					 rgb(252, 252, 252); border-radius: 5px; color: rgb(0, 0, 0);"><link rel="stylesheet" href="//cs3.wettercomassets.com/woys/5/css/w.css" media="all">
					<div class="wcom-city"><a style="color: #000" href="https://www.wetter.com/deutschland/boehmenkirch/DE0000520.html" target="_blank" 
					 rel="nofollow" aria-label="Wetter Berlin" title="Wetter Böhmenkirch">Wetter Böhmenkirch</a></div><div id="wcom-6ead28b0c2a8caade1dc54dbc8e72293-weather">
					</div>
					<script type="text/javascript" src="//cs3.wettercomassets.com/woys/5/js/w.js">
					</script>
					<script type="text/javascript">_wcomWidget({id: 'wcom-6ead28b0c2a8caade1dc54dbc8e72293',location: 'DE0000520',format: '300x250',type: 'summary'});
					</script>
					</div>

				</div>
				<div id="boxArticle">
					<article>
						<!--<p>  </p>-->
					</article>
					<?php 
					
						$free = shell_exec('free');
						$free = (string)trim($free);
						$free_arr = explode("\n", $free);
						$mem = explode(" ", $free_arr[1]);
						$mem = array_filter($mem);
						$mem = array_merge($mem);
						$memory_usage = floor($mem[2]/$mem[1]*100);
					?>	
					<p style="text-align:right;font-size:10px" ><?php echo "RAM usage: " . $memory_usage . "%";?></p>				
				</div>
	</section>
	<div id="boxFooter">
		<footer>
			<p>&copy; jettronics | Contact: <a href="mailto:jens.thielemann@gmail.com">Jens Thielemann</a>
			 | <a href="https://html5-templates.com/" target="_blank" rel="nofollow">HTML5 Templates</a>
			 | <a href="https://www.pushsafer.com/" target="_blank" rel="nofollow">Pushsafer</a></p>
		</footer>
	</div>
</body>

</html>
