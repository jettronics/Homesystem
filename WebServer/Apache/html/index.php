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
		#box1 { 
			position: relative; 
			backround: yellow;  
			height: 350px; width: 620px;    
			left: 0px; top: 0px; }    
		#boxWeather { 
			position: relative;   
			height: 350px; width: 300px;    
			left: 690px; top: -350px; } 			
		#boxArticle { 
			position: relative; 
			left: 0px; top: -300px; }  
		#boxFooter { 
			position: relative;  
			left: 0px; top: -320px; }  		      
	</style> 
    <!--<script src="script.js"></script>-->
    <script>
	</script>
    
</head>

<body>
	<?php
		$camsnapshot = "images/live1.jpg";
		shell_exec("/usr/bin/ffmpeg -loglevel fatal -i rtsp://admin:123456@192.168.178.48:554/live/ch0 -vframes 1 -y -r 1 images/live1.jpg 2>&1");
	?>
	<header>
		<div id="logo"><img src="images/logo.gif">Home System</div>
	</header>
	<section>
		<nav>  
			<ul>
				<!--<li><a href="indexa.php">Webradio</a>-->
				<li><a href="index.php">Webcam</a>
				<!--<li><a href="indexb.php">Detector</a>-->
			</ul>
		</nav>
	</section>
	<section id="pageContent">

				<div id="box1" style="text-align:center">
					<img src="<?php echo $camsnapshot; ?>" width="620" height="350" alt="webcam">
					<?php 
						//unset($camsnapshot);
						//echo memory_get_usage(TRUE);
					?>
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
