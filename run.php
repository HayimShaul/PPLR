#!/usr/bin/php
<?php

if (sizeof($argv) < 1) {
	print "missing prime parameter\n";
	print "Usage: <prime> <dim>\n";
	exit(1);
}
$p = $argv[1];

if (sizeof($argv) < 2) {
	print "missing min depth parameter\n";
	print "Usage: <prime> <dim>\n";
	exit(1);
}
$d = $argv[2];


$ok = false;
$L = 1;
while (!$ok && ($L < 10)) {
	$output = "out.$p.$d.$L";
	$cmd = "./test_helib --d=$d --p=$p --L=$L --l=1000 > $output 2>&1\n";

	print "runing $cmd\n";

	system("$cmd");

	$ok = `grep 'OK'  $output `;

	++$L;
}
