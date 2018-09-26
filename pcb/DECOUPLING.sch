EESchema Schematic File Version 4
LIBS:pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 10
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
P 5350 5400
F 0 "C1" H 5465 5446 50  0000 L CNN
F 1 "100n" H 5465 5355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5388 5250 50  0001 C CNN
F 3 "~" H 5350 5400 50  0001 C CNN
	1    5350 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5BA59950
P 5700 5400
F 0 "C3" H 5815 5446 50  0000 L CNN
F 1 "100n" H 5815 5355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5738 5250 50  0001 C CNN
F 3 "~" H 5700 5400 50  0001 C CNN
	1    5700 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 5BA5996E
P 6050 5400
F 0 "C4" H 6165 5446 50  0000 L CNN
F 1 "100n" H 6165 5355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 6088 5250 50  0001 C CNN
F 3 "~" H 6050 5400 50  0001 C CNN
	1    6050 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 5BA599BB
P 6400 5400
F 0 "C6" H 6515 5446 50  0000 L CNN
F 1 "100n" H 6515 5355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 6438 5250 50  0001 C CNN
F 3 "~" H 6400 5400 50  0001 C CNN
	1    6400 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C8
U 1 1 5BA599F7
P 6750 5400
F 0 "C8" H 6865 5446 50  0000 L CNN
F 1 "100n" H 6865 5355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 6788 5250 50  0001 C CNN
F 3 "~" H 6750 5400 50  0001 C CNN
	1    6750 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 5BA59A1B
P 7100 5400
F 0 "C9" H 7215 5446 50  0000 L CNN
F 1 "10u" H 7215 5355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 7138 5250 50  0001 C CNN
F 3 "~" H 7100 5400 50  0001 C CNN
	1    7100 5400
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR015
U 1 1 5BA5C380
P 7100 5100
F 0 "#PWR015" H 7100 4950 50  0001 C CNN
F 1 "+3.3V" H 7115 5273 50  0000 C CNN
F 2 "" H 7100 5100 50  0001 C CNN
F 3 "" H 7100 5100 50  0001 C CNN
	1    7100 5100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5BA5C3F3
P 7100 5700
F 0 "#PWR016" H 7100 5450 50  0001 C CNN
F 1 "GND" H 7105 5527 50  0000 C CNN
F 2 "" H 7100 5700 50  0001 C CNN
F 3 "" H 7100 5700 50  0001 C CNN
	1    7100 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 5100 7100 5250
Wire Wire Line
	7100 5550 7100 5700
Wire Wire Line
	7100 5250 6750 5250
Connection ~ 7100 5250
Connection ~ 5350 5250
Wire Wire Line
	5350 5250 4750 5250
Connection ~ 5700 5250
Wire Wire Line
	5700 5250 5350 5250
Connection ~ 6050 5250
Wire Wire Line
	6050 5250 5700 5250
Connection ~ 6400 5250
Wire Wire Line
	6400 5250 6050 5250
Connection ~ 6750 5250
Wire Wire Line
	6750 5250 6400 5250
Wire Wire Line
	7100 5550 6750 5550
Connection ~ 7100 5550
Connection ~ 5350 5550
Wire Wire Line
	5350 5550 4750 5550
Connection ~ 5700 5550
Wire Wire Line
	5700 5550 5350 5550
Connection ~ 6050 5550
Wire Wire Line
	6050 5550 5700 5550
Connection ~ 6400 5550
Wire Wire Line
	6400 5550 6050 5550
Connection ~ 6750 5550
Wire Wire Line
	6750 5550 6400 5550
Text HLabel 4750 5250 0    50   Input ~ 0
VDD_DEC
Text HLabel 4750 5550 0    50   Input ~ 0
VSS_DEC
Text HLabel 5500 3650 0    50   Input ~ 0
AVDD_DEC
$Comp
L power:+3.3V #PWR013
U 1 1 5BB282AB
P 6100 2800
F 0 "#PWR013" H 6100 2650 50  0001 C CNN
F 1 "+3.3V" H 6115 2973 50  0000 C CNN
F 2 "" H 6100 2800 50  0001 C CNN
F 3 "" H 6100 2800 50  0001 C CNN
	1    6100 2800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5BB28309
P 6100 4050
F 0 "#PWR014" H 6100 3800 50  0001 C CNN
F 1 "GND" H 6105 3877 50  0000 C CNN
F 2 "" H 6100 4050 50  0001 C CNN
F 3 "" H 6100 4050 50  0001 C CNN
	1    6100 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:C C7
U 1 1 5BB28370
P 6500 3800
F 0 "C7" H 6615 3846 50  0000 L CNN
F 1 "1uF" H 6615 3755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 6538 3650 50  0001 C CNN
F 3 "~" H 6500 3800 50  0001 C CNN
	1    6500 3800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 5BB2844D
P 6100 3800
F 0 "C5" H 6215 3846 50  0000 L CNN
F 1 "10nF" H 6215 3755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 6138 3650 50  0001 C CNN
F 3 "~" H 6100 3800 50  0001 C CNN
	1    6100 3800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5BB284D9
P 5700 3800
F 0 "C2" H 5815 3846 50  0000 L CNN
F 1 "10nF" H 5815 3755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5738 3650 50  0001 C CNN
F 3 "~" H 5700 3800 50  0001 C CNN
	1    5700 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 3650 6100 3650
Connection ~ 6100 3650
Wire Wire Line
	6100 3650 6500 3650
Wire Wire Line
	5700 3950 6100 3950
Connection ~ 6100 3950
Wire Wire Line
	6100 3950 6500 3950
$Comp
L Device:L L1
U 1 1 5BB2A405
P 6100 3050
F 0 "L1" H 6153 3096 50  0000 L CNN
F 1 "742792662 " H 6153 3005 50  0000 L CNN
F 2 "Inductor_SMD:L_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 6100 3050 50  0001 C CNN
F 3 "~" H 6100 3050 50  0001 C CNN
	1    6100 3050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5BB2A4B8
P 6100 3400
F 0 "R5" H 6170 3446 50  0000 L CNN
F 1 "1" H 6170 3355 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 6030 3400 50  0001 C CNN
F 3 "~" H 6100 3400 50  0001 C CNN
	1    6100 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 2800 6100 2900
Wire Wire Line
	6100 3200 6100 3250
Wire Wire Line
	6100 3550 6100 3650
Wire Wire Line
	5500 3650 5700 3650
Connection ~ 5700 3650
Wire Wire Line
	6100 3950 6100 4050
$Comp
L Device:C C?
U 1 1 5BB31FC4
P 8000 4300
AR Path="/5BB31FC4" Ref="C?"  Part="1" 
AR Path="/5BA5978A/5BB31FC4" Ref="C10"  Part="1" 
F 0 "C10" V 7748 4300 50  0000 C CNN
F 1 "1uF" V 7839 4300 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 8038 4150 50  0001 C CNN
F 3 "~" H 8000 4300 50  0001 C CNN
	1    8000 4300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5BB31FCB
P 8200 4300
AR Path="/5BB31FCB" Ref="#PWR?"  Part="1" 
AR Path="/5BA5978A/5BB31FCB" Ref="#PWR017"  Part="1" 
F 0 "#PWR017" H 8200 4050 50  0001 C CNN
F 1 "GND" V 8205 4172 50  0000 R CNN
F 2 "" H 8200 4300 50  0001 C CNN
F 3 "" H 8200 4300 50  0001 C CNN
	1    8200 4300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8200 4300 8150 4300
Text HLabel 7750 4300 0    50   Input ~ 0
DEC
Wire Wire Line
	7750 4300 7850 4300
$EndSCHEMATC
