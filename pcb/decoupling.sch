EESchema Schematic File Version 4
LIBS:pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:C C1
U 1 1 5BA59925
P 4700 3850
F 0 "C1" H 4815 3896 50  0000 L CNN
F 1 "100n" H 4815 3805 50  0000 L CNN
F 2 "" H 4738 3700 50  0001 C CNN
F 3 "~" H 4700 3850 50  0001 C CNN
	1    4700 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5BA59950
P 5050 3850
F 0 "C2" H 5165 3896 50  0000 L CNN
F 1 "100n" H 5165 3805 50  0000 L CNN
F 2 "" H 5088 3700 50  0001 C CNN
F 3 "~" H 5050 3850 50  0001 C CNN
	1    5050 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5BA5996E
P 5400 3850
F 0 "C3" H 5515 3896 50  0000 L CNN
F 1 "100n" H 5515 3805 50  0000 L CNN
F 2 "" H 5438 3700 50  0001 C CNN
F 3 "~" H 5400 3850 50  0001 C CNN
	1    5400 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 5BA599BB
P 5750 3850
F 0 "C4" H 5865 3896 50  0000 L CNN
F 1 "100n" H 5865 3805 50  0000 L CNN
F 2 "" H 5788 3700 50  0001 C CNN
F 3 "~" H 5750 3850 50  0001 C CNN
	1    5750 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 5BA599F7
P 6100 3850
F 0 "C5" H 6215 3896 50  0000 L CNN
F 1 "100n" H 6215 3805 50  0000 L CNN
F 2 "" H 6138 3700 50  0001 C CNN
F 3 "~" H 6100 3850 50  0001 C CNN
	1    6100 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 5BA59A1B
P 6450 3850
F 0 "C6" H 6565 3896 50  0000 L CNN
F 1 "10u" H 6565 3805 50  0000 L CNN
F 2 "" H 6488 3700 50  0001 C CNN
F 3 "~" H 6450 3850 50  0001 C CNN
	1    6450 3850
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0127
U 1 1 5BA5C380
P 6450 3550
F 0 "#PWR0127" H 6450 3400 50  0001 C CNN
F 1 "+3.3V" H 6465 3723 50  0000 C CNN
F 2 "" H 6450 3550 50  0001 C CNN
F 3 "" H 6450 3550 50  0001 C CNN
	1    6450 3550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0128
U 1 1 5BA5C3F3
P 6450 4150
F 0 "#PWR0128" H 6450 3900 50  0001 C CNN
F 1 "GND" H 6455 3977 50  0000 C CNN
F 2 "" H 6450 4150 50  0001 C CNN
F 3 "" H 6450 4150 50  0001 C CNN
	1    6450 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 3550 6450 3700
Wire Wire Line
	6450 4000 6450 4150
Wire Wire Line
	6450 3700 6100 3700
Connection ~ 6450 3700
Connection ~ 4700 3700
Wire Wire Line
	4700 3700 4100 3700
Connection ~ 5050 3700
Wire Wire Line
	5050 3700 4700 3700
Connection ~ 5400 3700
Wire Wire Line
	5400 3700 5050 3700
Connection ~ 5750 3700
Wire Wire Line
	5750 3700 5400 3700
Connection ~ 6100 3700
Wire Wire Line
	6100 3700 5750 3700
Wire Wire Line
	6450 4000 6100 4000
Connection ~ 6450 4000
Connection ~ 4700 4000
Wire Wire Line
	4700 4000 4100 4000
Connection ~ 5050 4000
Wire Wire Line
	5050 4000 4700 4000
Connection ~ 5400 4000
Wire Wire Line
	5400 4000 5050 4000
Connection ~ 5750 4000
Wire Wire Line
	5750 4000 5400 4000
Connection ~ 6100 4000
Wire Wire Line
	6100 4000 5750 4000
Text HLabel 4100 3700 0    50   Input ~ 0
VDD_DEC
Text HLabel 4100 4000 0    50   Input ~ 0
VSS_DEC
$EndSCHEMATC
