<?php 

if (isset($_GET[HEXColor])){

	$RGBvalue = $_GET[HEXColor];

	if (strlen($RGBvalue) != 6)
	{
		echo "Value error.";
		break;
	}
	else
	{
		// Breaks apart the HEX string (ex. FF00FF) and converts each color.

		//$redValue = hexdec(substr("$RGBvalue", 0, 2));
		//$greenValue = hexdec(substr("$RGBvalue", 2, 2));
		//$blueValue = hexdec(substr("$RGBvalue", 4, 2));
		$value = "1." . hexdec(substr("$RGBvalue", 0, 2)) . "." . hexdec(substr("$RGBvalue", 2, 2)) . "." . hexdec(substr("$RGBvalue", 4, 2));
		sendSerial($value);

	}

}

function sendSerial($data)
{
		//Set up serial open
		$fp = fopen("COM4", "w");
	    fwrite($fp, "$data");

	    //We're done, so close the serial port again
	    fclose($fp);
}

?>

<SCRIPT language="javascript">
function javaPHPCall(value){
	// the url which you have to reload is this page, but you add an action to the GET- or POST-variable
	var url="<?php echo $_SERVER[PHP_SELF];?>?HEXColor="+value;
	
	// Opens the url in the same window
	   window.open(url, "_self");
	}
  </SCRIPT>



<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd"> 
<html> 
<head> 
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"> 
<meta name="viewport" content="width=device-width; initial-scale=1.0; maximum-scale=1.0; user-scalable=0;" />
<title>35-07052 RGB LED Control</title> 
</head> 
<body> 
<h1>35-07052 RGB LED Control</h1> <br>
<script type="text/javascript" src="jscolor/jscolor.js"></script>
<input type="text" class="color" onchange="javaPHPCall(value)" value="<?php echo $RGBvalue;?>"/>
<p><a href="<?php echo $_SERVER[PHP_SELF];?>?HEXColor=000000">
Click here to turn all off.</a></p> 
<p><a href="<?php echo $_SERVER[PHP_SELF];?>?HEXColor=FFFFFF">
Click here to turn all on full.</a></p> 
<p><a href="<?php echo $_SERVER[PHP_SELF];?>?HEXColor=888888">
Click here to turn all half on.</a></p>

</body> 
</html>