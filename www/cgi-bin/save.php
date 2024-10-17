#!/usr/bin/php

<?php

$file = fopen("../data/strings.txt", "w");

fwrite($file, $_SERVER['QUERY_STRING']);

fclose($file);

?>