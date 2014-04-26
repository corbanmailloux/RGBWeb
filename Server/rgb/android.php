<?php 

if (isset($_POST[pwd]))
{
	$password = $_POST[pwd];
	if ($password == "*INSERT_PASSWORD_HERE*")
	{
		if (isset($_POST[servoAngle])){
			$angle = $_POST[servoAngle];
			if ($angle <= 1 & $angle >= 0)
			{
				$angle = "2." . $angle;
				sendSerial("$angle");
			}
			else 
			{
				echo "Invalid command value.";
			}
			
		}

		if (isset($_POST[rgbColor])){
			//$rgbValue = $_POST[rgbColor];
			$value = "1." . $_POST[rgbColor];
			sendSerial("$value");
		}
		
		if (isset ($_POST[crossfade])){
			$value = "3";
			sendSerial("$value");
		}
		
		if (isset ($_POST[reset])){
			$value = "0";
			sendSerial("$value");
		}

		if (isset ($_POST[christmas])){
			$value = "4";
			sendSerial("$value");
		}
	}
	else
	{
		echo "Incorrect password.";
	}
}
else
{
	echo "Password error.";
}

function sendSerial($data)
{
		//Set up serial open
		$fp = fopen("COM4", "w");
	    fwrite($fp, "$data");
		
		echo "1";

	    //We're done, so close the serial port again
	    fclose($fp);
}

?>
