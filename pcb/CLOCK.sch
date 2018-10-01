EESchema Schematic File Version 4
LIBS:pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 10
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
L Device:Crystal_GND23 Y?
U 1 1 5BAEAA1C
P 5800 3950
AR Path="/5BAEAA1C" Ref="Y?"  Part="1" 
AR Path="/5BAEA9B3/5BAEAA1C" Ref="Y1"  Part="1" 
F 0 "Y1" H 6000 4050 50  0000 L CNN
F 1 "Crystal_GND23" H 6000 3850 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_3225-4Pin_3.2x2.5mm_HandSoldering" H 5800 3950 50  0001 C CNN
F 3 "~" H 5800 3950 50  0001 C CNN
	1    5800 3950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5BAEAA23
P 5800 3650
AR Path="/5BAEAA23" Ref="#PWR?"  Part="1" 
AR Path="/5BAEA9B3/5BAEAA23" Ref="#PWR019"  Part="1" 
F 0 "#PWR019" H 5800 3400 50  0001 C CNN
F 1 "GND" H 5805 3477 50  0000 C CNN
F 2 "" H 5800 3650 50  0001 C CNN
F 3 "" H 5800 3650 50  0001 C CNN
	1    5800 3650
	-1   0    0    1   
$EndComp
Wire Wire Line
	5800 3650 5800 3750
$Comp
L power:+3.3V #PWR?
U 1 1 5BAEAA2A
P 6050 3950
AR Path="/5BAEAA2A" Ref="#PWR?"  Part="1" 
AR Path="/5BAEA9B3/5BAEAA2A" Ref="#PWR020"  Part="1" 
F 0 "#PWR020" H 6050 3800 50  0001 C CNN
F 1 "+3.3V" V 6065 4078 50  0000 L CNN
F 2 "" H 6050 3950 50  0001 C CNN
F 3 "" H 6050 3950 50  0001 C CNN
	1    6050 3950
	0    1    1    0   
$EndComp
Wire Wire Line
	5950 3950 6050 3950
$Comp
L power:+3.3V #PWR?
U 1 1 5BAEAA31
P 5550 3950
AR Path="/5BAEAA31" Ref="#PWR?"  Part="1" 
AR Path="/5BAEA9B3/5BAEAA31" Ref="#PWR018"  Part="1" 
F 0 "#PWR018" H 5550 3800 50  0001 C CNN
F 1 "+3.3V" V 5565 4078 50  0000 L CNN
F 2 "" H 5550 3950 50  0001 C CNN
F 3 "" H 5550 3950 50  0001 C CNN
	1    5550 3950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5550 3950 5650 3950
Text HLabel 5800 4450 3    50   Input ~ 0
HFXTAL_N
Wire Wire Line
	5800 4150 5800 4450
$EndSCHEMATC
