<?php
	require_once ("inc/database.php");
	require_once ("inc/class.php");
?>
<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8" />
		 <link rel="stylesheet" type="text/css" href="style.css">
		<title>Test server</title>
	</head>
	
	<body>
		<header>
			<h1>Test Lab</h1>
		</header>
		
		<nav>
			<ul>
				<li><a href="index.php?page=compile">Compile firmware</a></li>
				<li><a href="index.php?page=test">Test router</a></li>
				<li><a href="index.php?page=log">Log</a></li>
			</ul>		
		</nav>
		
		
		<?php		
			if (isset($_GET['page'])) { 
				include file_exists("./inc/{$_GET['page']}.php") == true ?  
				dirname(__FILE__)."/inc/{$_GET['page']}.php" :  
				dirname(__FILE__)."/inc/404.php"; 
			} else { 
				include dirname(__FILE__)."/inc/compile.php"; 
			}
		?>
					
	</body>
</html>
