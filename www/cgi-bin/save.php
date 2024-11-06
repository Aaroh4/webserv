#!/usr/bin/php

<?php

$file_path = realpath(__DIR__ . '/../data/strings.txt');

if ($file = fopen($file_path, "a")) 
{
	$sanitized_query_string = strip_tags($_SERVER['QUERY_STRING']);
	$sanitized_query_string = htmlspecialchars($sanitized_query_string, ENT_QUOTES, 'UTF-8');
	parse_str($sanitized_query_string, $params);
	$name = isset($params['input']) ? strip_tags($params['input']) : '';
	fwrite($file, $name);
	fwrite($file, "\n");
	fclose($file);
    echo $name . " written to file.";
} 
else 
{
    echo "Failed to open file for writing.";
}

?>
