EESchema Schematic File Version 4
LIBS:pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 8 10
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
L localization-symbols:lidar U?
U 1 1 5BB1F813
P 5500 4050
AR Path="/5BB1F813" Ref="U?"  Part="1" 
AR Path="/5BB1F7DF/5BB1F813" Ref="U4"  Part="1" 
F 0 "U4" V 5599 4578 50  0000 L CNN
F 1 "lidar" V 5690 4578 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x07_P2.54mm_Vertical" H 5500 4050 50  0001 C CNN
F 3 "" H 5500 4050 50  0001 C CNN
	1    5500 4050
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR025
U 1 1 5BB1F85C
P 5300 3850
F 0 "#PWR025" H 5300 3600 50  0001 C CNN
F 1 "GND" V 5305 3722 50  0000 R CNN
F 2 "" H 5300 3850 50  0001 C CNN
F 3 "" H 5300 3850 50  0001 C CNN
	1    5300 3850
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR026
U 1 1 5BB1F8C1
P 5300 3950
F 0 "#PWR026" H 5300 3800 50  0001 C CNN
F 1 "VCC" V 5318 4077 50  0000 L CNN
F 2 "" H 5300 3950 50  0001 C CNN
F 3 "" H 5300 3950 50  0001 C CNN
	1    5300 3950
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR028
U 1 1 5BB1F928
P 5300 4550
F 0 "#PWR028" H 5300 4300 50  0001 C CNN
F 1 "GND" V 5305 4422 50  0000 R CNN
F 2 "" H 5300 4550 50  0001 C CNN
F 3 "" H 5300 4550 50  0001 C CNN
	1    5300 4550
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR027
U 1 1 5BB1F99D
P 5300 4350
F 0 "#PWR027" H 5300 4200 50  0001 C CNN
F 1 "VCC" V 5318 4477 50  0000 L CNN
F 2 "" H 5300 4350 50  0001 C CNN
F 3 "" H 5300 4350 50  0001 C CNN
	1    5300 4350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5300 3850 5400 3850
Wire Wire Line
	5300 3950 5400 3950
Wire Wire Line
	5300 4350 5400 4350
Wire Wire Line
	5300 4550 5400 4550
Text HLabel 5300 4050 0    50   Input ~ 0
RX
Text HLabel 5300 4150 0    50   Input ~ 0
TX
Text HLabel 5300 4450 0    50   Input ~ 0
MOTO_CTRL
Wire Wire Line
	5300 4450 5400 4450
Wire Wire Line
	5300 4150 5400 4150
Wire Wire Line
	5300 4050 5400 4050
$EndSCHEMATC
