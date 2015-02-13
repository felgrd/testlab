<article>	
	<h2>Releases of firmware</h2>	
	<ul>
	<?php	
		$database = new database();
		$releases = $database->select_releases();
		
		if($releases != null){
			foreach ($releases as $release){
				echo "<li>";
				echo "<a href=\"index.php?page=test&amp;release=".$release->id."\">";
				echo $release->date;
				echo "</a></li>";
			}
		}else{
			echo "No releases available.";
		}
	?>
	</ul>
</article>


<article>	
	
	<?php
	
	if(isset($_GET["release"]) && is_numeric($_GET["release"])){
	
		echo "<h2>Tests of firmware</h2>";
		echo "<table>";
		echo "<tr>";
		echo "<th>Routers/Tests</th>";
		
		$routers = $database->select_routers();
		
		foreach ($routers as $router){
			echo "<td>";
			echo $router->name;
			echo "</td>";			
		}

		echo "</tr>";

		$procedures = $database->select_procedures();
		$tests = $database->select_tests($_GET["release"]);
		
		foreach ($procedures as $procedure){
			echo "<tr>";
			echo "<td>";
			echo $procedure->name;			
			
			
			foreach ($routers as $router){
				echo "<td class=\"test\">";
				
				if(isset($tests[$procedure->id][$router->id])){
					if($tests[$procedure->id][$router->id] == 1){
						echo "<img src=\"images/check.gif\" alt=\"yes\">";
					}else{
						echo "<img src=\"images/delete.png\" width=\"10\" alt=\"no\">";
					}				
				}else{
					echo "No test";
				}				
				
				echo "</td>";
			}			
			echo "</tr>";
		}
		
		echo "</table>";

		}
	?>
</article>
