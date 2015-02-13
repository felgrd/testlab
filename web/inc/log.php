<article>
	
	<?php
	
	echo "<h2>Logs</h2>";
	

	$database = new database();
	$logs = $database->select_logs();
		
	if($logs != null){
		
		echo "<table>";
		echo "<tr>";
		echo "<th>Relase</th>";
		echo "<th>Router</th>";
		echo "<th>Procedure</th>";
		echo "<th>Message</th>";
		echo "</tr>";
		
		foreach ($logs as $log){
			echo "<tr>";		
			echo "<td class=\"center\">" . $log->release . "</td>";
			echo "<td class=\"center\">" . $log->router . "</td>";
			echo "<td class=\"center\">" . $log->procedure . "</td>";
			echo "<td>" . $log->event . "</td>";
			echo "</tr>";			
		}
	}else{		
		echo "<p>No message.</p>";		
	}

	echo "</table>";
	?>

</article>
