<?php 
	
	 $database = "fyp";
	 $uname = "root";
	 $pswd = "";
	 $server = "localhost";

	 $conn = mysqli_connect($server,$uname,$pswd,$database);

	 if($conn){

	 	echo "Entered Password is";
	 }

	 else{

	 	die("failed to connect");
	 }


	 if (isset($_POST['username']) && isset($_POST['password'])) {
	 		
	 		$username = $_POST['username'];
	 		$password = $_POST['password'];

 	$query = "select * from login where username = '$username' and password = '$password'";
	 	$result = mysqli_query($conn, $query);

	 	$row = mysqli_fetch_assoc($result);

	 	if ($row) {
	 		
	 		// header("Location: http://www.example.com");
	 		header("Location: Web.html");
	 	}

	 	else{

	 		echo " Wrong";
			echo"  Please try again !";
	 	}


	 }
 ?>