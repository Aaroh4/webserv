#!/usr/bin/php
<?php

$query = isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : '';
parse_str($query, $params);
$name = isset($params['input']) ? strip_tags($params['input']) : '';
$timeout = isset($_SERVER['TIMEOUT']) ? $_SERVER['TIMEOUT'] : '';
$response = "<!DOCTYPE html>\n<html>\n<body>\n<h1>Hello $name!</h1>\n</body>\n</html>";
$contentLength = strlen($response);

echo "HTTP/1.1 200 OK\n";
echo "Content-Type: text/html\n";
echo "Content-Length: $contentLength\n";
echo "Connection: Keep-Alive\n";
echo "Keep-Alive: timeout=$timeout, max=100\n\n";
echo $response;
?>