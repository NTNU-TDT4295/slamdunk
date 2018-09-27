EESchema Schematic File Version 4
LIBS:pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 10
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
L Connector_Generic:Conn_02x10_Odd_Even J1
U 1 1 5BAAE2EF
P 5850 3950
F 0 "J1" H 5900 4567 50  0000 C CNN
F 1 "Conn_02x10_Odd_Even" H 5900 4476 50  0000 C CNN
F 2 "Connector_Multicomp:Multicomp_MC9A12-2034_2x10_P2.54mm_Vertical" H 5850 3950 50  0001 C CNN
F 3 "~" H 5850 3950 50  0001 C CNN
	1    5850 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 3650 6150 3750
Connection ~ 6150 3750
Wire Wire Line
	6150 3750 6150 3850
Connection ~ 6150 3850
Wire Wire Line
	6150 3850 6150 3950
Connection ~ 6150 3950
Wire Wire Line
	6150 3950 6150 4050
Connection ~ 6150 4050
Wire Wire Line
	6150 4050 6150 4150
Connection ~ 6150 4150
Wire Wire Line
	6150 4150 6150 4250
Connection ~ 6150 4250
Wire Wire Line
	6150 4250 6150 4350
Connection ~ 6150 4350
Wire Wire Line
	6150 4350 6150 4450
Connection ~ 6150 4450
$Comp
L power:GND #PWR019
U 1 1 5BAAE306
P 6150 4600
F 0 "#PWR019" H 6150 4350 50  0001 C CNN
F 1 "GND" H 6155 4427 50  0000 C CNN
F 2 "" H 6150 4600 50  0001 C CNN
F 3 "" H 6150 4600 50  0001 C CNN
	1    6150 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 4450 6150 4600
Text HLabel 5550 4250 0    50   Input ~ 0
RST
Text HLabel 5550 4150 0    50   Input ~ 0
SWO
Text HLabel 5550 3950 0    50   Input ~ 0
SWCLK
Text HLabel 5550 3850 0    50   Input ~ 0
SWDIO
Wire Wire Line
	5550 3850 5650 3850
Wire Wire Line
	5550 3950 5650 3950
Wire Wire Line
	5550 4150 5650 4150
Wire Wire Line
	5550 4250 5650 4250
$Comp
L power:+3.3V #PWR018
U 1 1 5BACAB50
P 5550 3550
F 0 "#PWR018" H 5550 3400 50  0001 C CNN
F 1 "+3.3V" V 5565 3678 50  0000 L CNN
F 2 "" H 5550 3550 50  0001 C CNN
F 3 "" H 5550 3550 50  0001 C CNN
	1    5550 3550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5550 3550 5650 3550
$EndSCHEMATC
