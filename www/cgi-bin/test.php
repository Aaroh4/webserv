#!/usr/bin/php
<?php

// Check if QUERY_STRING exists and sanitize it
$query = isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : '';

// Parse the query string to get only the value
parse_str($query, $params);
$name = isset($params['input']) ? strip_tags($params['input']) : '';

// Echo "Hello!" followed by the sanitized input value
echo "Hello! " . $name;
?>
