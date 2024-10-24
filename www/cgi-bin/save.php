#!/usr/bin/php

<?php

$file_path = realpath(__DIR__ . '/../data/strings.txt');

if ($file = fopen($file_path, "a")) 
{
	 $sanitized_query_string = strip_tags($_SERVER['QUERY_STRING']);
	 $sanitized_query_string = htmlspecialchars($sanitized_query_string, ENT_QUOTES, 'UTF-8');
	 fwrite($file, $sanitized_query_string);
	 fwrite($file, "\n");
	 fclose($file);
    echo "Query string successfully written to file.";
} 
else 
{
    echo "Failed to open file for writing.";
}

?>
