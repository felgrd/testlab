<?php

class Release{
	public $id;
	public $date;
	public $type;		
}

class Build{
	public $id;
	public $state;
	public $product;
	public $platform;
	public $release;	
}

class Router{
	public $id;
	public $name;
	public $port;
	public $address;
	public $product;
	public $platform;	
}

class Procedure{
	public $id;
	public $name;
	public $unit;
	public $functions;	
}

class Log{
	public $id;
	public $release;
	public $router;
	public $procedure;
	public $event;
}

?>
