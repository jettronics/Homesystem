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
    <script>
	</script>
    
</head>

<body>
	<?php
		//$infoRowText = sendcmd("radiostatus");
		
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
		
		if( empty($_POST) == false ) {
			foreach($_POST as $k => $v) { 
				$infoRowText = sendcmd($v);
			}
		} else {
			$infoRowText = sendcmd("radiostatus");
		}
		
		$stationcount = substr_count($infoRowText, ';')-1;
		
		// Get first number
		preg_match('!\d+!', $infoRowText, $numbers);

		$colorStationArray = array();
		for ($i = 1; $i < $stationcount; $i++) {
			$colorStationArray[$i] = "AEC6CF";
			if( intval($numbers[0]) > 0 ) {
				$colorOff = "66ce64";
				if( $i == (intval($numbers[0])) ) {
					$colorStationArray[$i] = "66ce64";
				}
			} else {
				$colorOff = "df9696";
			}
		}

		if( stripos($infoRowText, "Kitchen: 0") !== false ) {
			$colorKitchen = "df9696";
		} else {
			$colorKitchen = "66ce64";
		}

		if( stripos($infoRowText, "Bathroom: 0") !== false ) {
			$colorBathroom = "df9696";
		} else {
			$colorBathroom = "66ce64";
		}
		
		// Use $stationNamesArray[x]
		$stationNamesArray = explode(";", $infoRowText);
	
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

    		<!--<main>-->
				<div id="boxSource" style="text-align:center">
					<form method="post">
					
					<?php 
						for ($i = 1; $i < $stationcount; $i++) {
							$j = $i+1;
							echo '<button type="submit" name="station='.$j.'" id="station='.$j.'" value="station='.$j.'"';
							echo 'style="background:#'.$colorStationArray[$i].';height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">'.$stationNamesArray[$j].'</button>';
						}
					?>
					<!--
					<button type="submit" name="station=1" id="station=1" value="station=1"
					 style="background:#<?php echo $colorStationArray[2]; ?>;height:50px;
					 width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">
					 <?php echo $stationNamesArray[2]; ?></button>
					<button type="submit" name="station=2" id="station=2" value="station=2"
					 style="background:#<?php echo $colorStationArray[3]; ?>;height:50px;
					 width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">
					 <?php echo $stationNamesArray[3]; ?></button>
					<button type="submit" name="melodicrock" id="melodicrock" value="melodicrock"
					 style="background:#<?php echo $colorMelodicRock; ?>;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Melodic Rock</button>
					<button type="submit" name="lounge" id="lounge" value="lounge"
					 style="background:#<?php echo $colorLounge; ?>;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Lounge</button>
					<button type="submit" name="house" id="house" value="house"
					 style="background:#<?php echo $colorHouse; ?>;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">House</button>
					<button type="submit" name="top100" id="top100" value="top100"
					 style="background:#<?php echo $colorTop100; ?>;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Top 100</button>
					<button type="submit" name="bestof80s" id="bestof80s" value="bestof80s"
					 style="background:#<?php echo $colorBestOf80s; ?>;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Best of 80s</button>
					</form>
					-->
				</div>
				<div id="boxControl" style="text-align:center">
					<form method="post">
					<button type="submit" name="radiostatus" id="radiostatus" value="radiostatus"
					 style="background:#BEC6CF;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Info</button>
					<button type="submit" name="switchoff" id="switchoff" value="switchoff"
					 style="background:#<?php echo $colorOff; ?>;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Off</button>
					<button type="submit" name="kitchenspeaker" id="kitchenspeaker" value="kitchenspeaker"
					 style="background:#<?php echo $colorKitchen; ?>;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Kitchen</button>
					<button type="submit" name="bathroomspeaker" id="bathroomspeaker" value="bathroomspeaker"
					 style="background:#<?php echo $colorBathroom; ?>;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Bathroom</button>
					<button type="submit" name="volumeplus" id="volumeplus" value="volumeplus"
					 style="background:#BEC6CF;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Volume +</button>
					<button type="submit" name="volumeminus" id="volumeminus" value="volumeminus"
					 style="background:#BEC6CF;height:50px;width:150px;font-size:15px;font-weight:bold;color:#444;border-radius:10px;">Volume -</button>
					</form>
				</div>	
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
						<!--<p> </p>-->
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
