<?php

require_once ("class.php");

class database
{

	protected $connection;
	
	public function __construct()
	{
		// Pripojeni do databaze
		$this->connection = new mysqli("localhost", "test", "test");
				
		if($this->connection->connect_errno){						
			echo "DB connection: ".$this->connection->connect_error;
			exit();		
		}
		
		// Vyber databaze
		$this->connection->select_db("test");		
	}
	
	public function __destruct()
	{
		// Ukonceni spojeni databaze
		$this->connection->close();
	}
	
	public function select_releases()
	{
		$releases = null;
		$sql = "SELECT * FROM fwreleases ORDER by idfwreleases DESC";
		if($result = $this->connection->query($sql)){
			
			while($release = $result->fetch_object()){
				$releases[$release->idfwreleases] = new Release();
				$releases[$release->idfwreleases]->id = $release->idfwreleases;
				$releases[$release->idfwreleases]->date = $release->date;
				$releases[$release->idfwreleases]->type = $release->type;		
			}
			$result->close();
			
		}else{			
			echo $this->connection->error;
			exit();
		}
		
		return $releases;
	}
	
	public function select_builds($fwrelease = NULL)
	{
		$builds = null;		
		$sql = "SELECT builds_product.idbuilds AS id,".
		" builds_product.state AS build_state,".
		" products.name AS product_name,".
		" platforms.name AS platform_name".
		" FROM builds_product". 
		" LEFT JOIN products ON builds_product.idproducts = products.idproducts".
		" LEFT JOIN platforms ON products.idplatforms = platforms.idplatforms".
		" WHERE builds_product.idfwreleases=".$fwrelease."".
		" ORDER BY platforms.idplatforms, products.idproducts";
		
		if($result = $this->connection->query($sql)){
			
			while($build = $result->fetch_object()){
				$builds[$build->id] = new Build();
				$builds[$build->id]->id = $build->id;
				$builds[$build->id]->state = $build->build_state;
				$builds[$build->id]->product = $build->product_name;
				$builds[$build->id]->platform = $build->platform_name;
			}
			$result->close();
			
		}else{			
			echo $this->connection->error;
			exit();
		}	
		
		return $builds;
	}
	
	# Vyber vsech routeru z databaze
	public function select_routers(){
		$routers = null;
		$sql = "SELECT * FROM routers";

		if($result = $this->connection->query($sql)){

			while($router = $result->fetch_object()){
				$routers[$router->idrouters] = new Router();
				$routers[$router->idrouters]->id = $router->idrouters;
				$routers[$router->idrouters]->name = $router->name;
				$routers[$router->idrouters]->port = $router->port;
				$routers[$router->idrouters]->address = $router->address;
			}
			$result->close();
			
		}else{			
			echo $this->connection->error;
			exit();
		}	
		
		return $routers;		
	}
	
	# Vyber vsech testovacich procedur z databaze
	public function select_procedures(){
		$procedures = null;
		$sql = "SELECT * FROM procedures";
		
		if($result = $this->connection->query($sql)){

			while($procedure = $result->fetch_object()){
				$procedures[$procedure->idprocedures] = new Procedure();
				$procedures[$procedure->idprocedures]->id = $procedure->idprocedures;
				$procedures[$procedure->idprocedures]->name = $procedure->name;
			}
			$result->close();
			
		}else{			
			echo $this->connection->error;
			exit();
		}	
		
		return $procedures;	
	}
	
	# Vyber vsech testu na vsech routerech
	public function select_tests($fwrelease = NULL){
		$tests = null;
		$sql = "SELECT * from tests_procedure WHERE idfwreleases=".$fwrelease;
		
		if($result = $this->connection->query($sql)){

			while($test = $result->fetch_object()){
				$tests[$test->idprocedures][$test->idrouters] = $test->result;
			}
			$result->close();
			
		}else{			
			echo $this->connection->error;
			exit();
		}	
		
		return $tests;	
		
	}
	
	# Vyber logu
	public function select_logs(){
		$logs = null;
		$sql = "SELECT * from logs ORDER BY idlogs DESC";
		
		if($result = $this->connection->query($sql)){
			
			while($log = $result->fetch_object()){
				$logs[$log->idlogs] = new Log();
				$logs[$log->idlogs]->id = $log->idlogs;
				$logs[$log->idlogs]->release = $log->idfwreleases;
				$logs[$log->idlogs]->router = $log->idrouters;
				$logs[$log->idlogs]->procedure = $log->idprocedures;
				$logs[$log->idlogs]->event = $log->event;
			}
			$result->close();
					
		}else{			
			echo $this->connection->error;
			exit();
		}
		
		return $logs;
	}
}

?>
