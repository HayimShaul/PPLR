<?php

print "all: all2\n\n";

$all = "";

#foreach (Array(4, 8, 16, 32, 64, 128) as $d) {
#	foreach (Array(10, 20, 30, 40, 50, 60, 70, 80, 90, 100) as $l) {
#		if ($l < $d)
#			continue;

$K = 1000;
$M = $K*$K;
foreach (Array( [32,50], [16,60], [8,40] ) as $dn) {
		$d = $dn[0];
		$l = $dn[1];
		foreach (Array(10*$K, 100*$K, 1*$M, 10*$M, 100*$M) as $p) {
			$output = "out.$p.$l.$d";
			$all .= " $output";
			print "$output:\n";
			print "\t./test_helib --d=$d --p=$p --l=$l > $output 2>&1\n";
			print "\n";
		}
}
#	}
#}

print "all2: $all\n\n";
