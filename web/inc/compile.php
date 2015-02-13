<article>	
	<h2>Releases of firmware</h2>	
	<ul>
	<?php	
		$database = new database();
		$releases = $database->select_releases();
		
		if($releases != null){
			foreach ($releases as $release){
				echo "<li>";
				echo "<a href=\"index.php?page=compile&amp;release=".$release->id."\">";
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
	
		echo "<h2>Compiles of firmware</h2>\n";		
			
		if(isset($_GET["release"]) && is_numeric($_GET["release"])){	
							
			$builds = $database->select_builds($_GET["release"]);
			
			if($builds != null){
				
				echo "<table>\n";
				echo "<tr>\n";
				echo "<th>Product</th>\n";
				echo "<th>Platform</th>\n";
				echo "<th>Compile</th>\n";
				echo "</tr>\n";
								
				foreach ($builds as $build){
					echo "<tr>\n";
					echo "<td>".$build->product."</td>\n";
					echo "<td>".$build->platform."</td>\n";
					echo "<td class=\"center\">";
					if($build->state == 1){
						echo "<img src=\"images/check.gif\" alt=\"yes\">";
					}else{
						echo "<img src=\"images/delete.png\" width=\"10\" alt=\"no\">";
					}						
					echo "</td>\n";
					echo "</tr>\n";
				}
			
			}else{				
				echo "<p>No build.</p>";
			}	
		}		
			
		echo "</table>\n";
	}
	?>	
</article>	
