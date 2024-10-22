EESchema Schematic File Version 4
LIBS:pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 9
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 5250 3600 1    50   Input ~ 0
LV
Text HLabel 5900 3600 1    50   Input ~ 0
HV
Text HLabel 5150 4300 0    50   Input ~ 0
LV1
Text HLabel 6700 4300 0    50   Input ~ 0
LV2
Text HLabel 6050 4300 2    50   Input ~ 0
HV1
Text HLabel 7600 4300 2    50   Input ~ 0
HV2
$Comp
L Device:R R6
U 1 1 5BB7C451
P 5250 3950
F 0 "R6" H 5320 3996 50  0000 L CNN
F 1 "10k" H 5320 3905 50  0000 L CNN
F 2 "" V 5180 3950 50  0001 C CNN
F 3 "~" H 5250 3950 50  0001 C CNN
	1    5250 3950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 5BB7C4BE
P 5900 3950
F 0 "R7" H 5970 3996 50  0000 L CNN
F 1 "10k" H 5970 3905 50  0000 L CNN
F 2 "" V 5830 3950 50  0001 C CNN
F 3 "~" H 5900 3950 50  0001 C CNN
	1    5900 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 4000 5500 3800
Wire Wire Line
	5500 3800 5250 3800
Wire Wire Line
	5250 4100 5250 4300
Wire Wire Line
	5250 4300 5150 4300
Wire Wire Line
	5400 4300 5250 4300
Connection ~ 5250 4300
Wire Wire Line
	5800 4300 5900 4300
Wire Wire Line
	5900 4100 5900 4300
Connection ~ 5900 4300
Wire Wire Line
	5900 4300 6050 4300
Wire Wire Line
	5900 3800 5900 3600
Wire Wire Line
	5250 3800 5250 3600
Connection ~ 5250 3800
Text HLabel 6800 3600 1    50   Input ~ 0
LV
Text HLabel 7450 3600 1    50   Input ~ 0
HV
$Comp
L Device:R R8
U 1 1 5BB7C92B
P 6800 3950
F 0 "R8" H 6870 3996 50  0000 L CNN
F 1 "10k" H 6870 3905 50  0000 L CNN
F 2 "" V 6730 3950 50  0001 C CNN
F 3 "~" H 6800 3950 50  0001 C CNN
	1    6800 3950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R9
U 1 1 5BB7C932
P 7450 3950
F 0 "R9" H 7520 3996 50  0000 L CNN
F 1 "10k" H 7520 3905 50  0000 L CNN
F 2 "" V 7380 3950 50  0001 C CNN
F 3 "~" H 7450 3950 50  0001 C CNN
	1    7450 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 4000 7050 3800
Wire Wire Line
	7050 3800 6800 3800
Wire Wire Line
	6800 4100 6800 4300
Wire Wire Line
	6800 4300 6700 4300
Wire Wire Line
	6950 4300 6800 4300
Connection ~ 6800 4300
Wire Wire Line
	7350 4300 7450 4300
Wire Wire Line
	7450 4100 7450 4300
Connection ~ 7450 4300
Wire Wire Line
	7450 4300 7600 4300
Wire Wire Line
	7450 3800 7450 3600
Wire Wire Line
	6800 3800 6800 3600
Connection ~ 6800 3800
$Comp
L Transistor_FET:BSS138 Q1
U 1 1 5BAAF42A
P 5600 4200
F 0 "Q1" V 5850 4200 50  0000 C CNN
F 1 "BSS138" V 5941 4200 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5800 4125 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/BS/BSS138.pdf" H 5600 4200 50  0001 L CNN
	1    5600 4200
	0    1    1    0   
$EndComp
Wire Wire Line
	5500 4000 5600 4000
$Comp
L Transistor_FET:BSS138 Q2
U 1 1 5BAAF684
P 7150 4200
F 0 "Q2" V 7400 4200 50  0000 C CNN
F 1 "BSS138" V 7491 4200 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 7350 4125 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/BS/BSS138.pdf" H 7150 4200 50  0001 L CNN
	1    7150 4200
	0    1    1    0   
$EndComp
Wire Wire Line
	7050 4000 7150 4000
$EndSCHEMATC
