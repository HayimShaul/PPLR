#!/usr/bin/php
<?php

print "all: all2\n\n";

$all = "";

$K = 1000;
$M = $K*$K;


#function act($p, $L, $d) {
#	global $all;
#	$output = "out.$p.$d.$L";
#	$all .= " $output";
#	print "$output:\n";
#	print "\t./test_helib --d=$d --p=$p --L=$L > $output 2>&1\n";
#	print "\n";
#}

function act($p, $L, $d) {
	global $all;
	$output = "out.$p.$d";
	$all .= " $output";
	print "$output:\n";
	print "\t./run.php $p $d\n";
	print "\n";
}

act(2, 3, 20);
act(5, 3, 20);
act(17, 3, 20);
act(101, 3, 20);
act(1031, 3, 20);
act(79999, 3, 20);
act(1190492669, 5, 20);
act(1000000000039, 5, 20);
act(7625597485003, 5, 20);
act(28722900390631, 7, 20);
act(111111151111111, 7, 20);
act(888888877777777, 7, 20);
act(1333333333333333, 7, 20);
act(1000000000000000003, 8, 20);

act(2, 3, 30);
act(5, 3, 30);
act(17, 3, 30);
act(101, 3, 30);
act(1031, 3, 30);
act(79999, 3, 30);
act(1190492669, 5, 30);
act(1000000000039, 5, 30);
act(7625597485003, 5, 30);
act(28722900390631, 7, 30);
act(111111151111111, 7, 30);
act(888888877777777, 7, 30);
act(1333333333333333, 7, 30);
act(1000000000000000003, 8, 30);

act(2, 3, 40);
act(5, 3, 40);
act(17, 3, 40);
act(101, 3, 40);
act(1031, 3, 40);
act(79999, 3, 40);
act(1190492669, 5, 40);
act(1000000000039, 5, 40);
act(7625597485003, 5, 40);
act(28722900390631, 7, 40);
act(111111151111111, 7, 40);
act(888888877777777, 7, 40);
act(1333333333333333, 7, 40);
act(1000000000000000003, 8, 40);

#foreach ([1000, 10000, 100000] as $l) {
#	foreach ([10, 20, 30, 40] as $d) {
#		foreach ([10, 100, 1000, 10*$K, 100*$K, 1000*$K, 10*$M] as $p) {
#			foreach ([1,2,3,4,5] as $L) {
#				$output = "out.$p.$l.$d.$L";
#				$all .= " $output";
#				print "$output:\n";
#				print "\t./test_helib --d=$d --p=$p --l=$l --L=$L > $output 2>&1\n";
#				print "\n";
#			}
#		}
#	}
#}

#foreach (Array(4, 8, 16, 32, 64, 128) as $d) {
#	foreach (Array(10, 20, 30, 40, 50, 60, 70, 80, 90, 100) as $l) {
#		if ($l < $d)
#			continue;


#foreach (Array( [32,50], [16,60], [8,40] ) as $dn) {
#		$d = $dn[0];
#		$l = $dn[1];
#		foreach (Array(10*$K, 100*$K, 1*$M, 10*$M, 100*$M) as $p) {
#			$output = "out.$p.$l.$d";
#			$all .= " $output";
#			print "$output:\n";
#			print "\t./test_helib --d=$d --p=$p --l=$l > $output 2>&1\n";
#			print "\n";
#		}
#}
##	}
##}

print "all2: $all\n\n";
