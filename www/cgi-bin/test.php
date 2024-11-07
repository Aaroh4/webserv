#!/usr/bin/php
<?php

$query = isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : '';
parse_str($query, $params);
$name = isset($params['input']) ? strip_tags($params['input']) : '';

echo "Hello! " . $name;
?>
