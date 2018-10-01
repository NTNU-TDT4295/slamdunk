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
P 5750 4050
AR Path="/5BB1F813" Ref="U?"  Part="1" 
AR Path="/5BB1F7DF/5BB1F813" Ref="U4"  Part="1" 
F 0 "U4" V 5849 4578 50  0000 L CNN
F 1 "lidar" V 5940 4578 50  0000 L CNN
F 2 "Connector_Molex:Molex_SPOX_5267-07A_1x07_P2.50mm_Vertical" H 5750 4050 50  0001 C CNN
F 3 "" H 5750 4050 50  0001 C CNN
	1    5750 4050
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR027
U 1 1 5BB1F85C
P 5650 3850
F 0 "#PWR027" H 5650 3600 50  0001 C CNN
F 1 "GND" V 5655 3722 50  0000 R CNN
F 2 "" H 5650 3850 50  0001 C CNN
F 3 "" H 5650 3850 50  0001 C CNN
	1    5650 3850
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR026
U 1 1 5BB1F928
P 5400 5100
F 0 "#PWR026" H 5400 4850 50  0001 C CNN
F 1 "GND" V 5405 4972 50  0000 R CNN
F 2 "" H 5400 5100 50  0001 C CNN
F 3 "" H 5400 5100 50  0001 C CNN
	1    5400 5100
	0    -1   -1   0   
$EndComp
Text HLabel 5650 4050 0    50   Input ~ 0
RX
Text HLabel 5650 3950 0    50   Input ~ 0
TX
Text HLabel 5650 4350 0    50   Input ~ 0
MOTO_CTRL
$Comp
L Device:Battery BT?
U 1 1 5BB02470
P 3100 4900
AR Path="/5BB02470" Ref="BT?"  Part="1" 
AR Path="/5BB1F7DF/5BB02470" Ref="BT1"  Part="1" 
F 0 "BT1" H 3208 4946 50  0000 L CNN
F 1 "Battery" H 3208 4855 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" V 3100 4960 50  0001 C CNN
F 3 "~" V 3100 4960 50  0001 C CNN
	1    3100 4900
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NC_Dual JP9
U 1 1 5BB0256D
P 3100 4450
F 0 "JP9" V 3146 4552 50  0000 L CNN
F 1 "Jumper_NC_Dual" V 3055 4552 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3100 4450 50  0001 C CNN
F 3 "~" H 3100 4450 50  0001 C CNN
	1    3100 4450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3100 5100 3900 5100
$Comp
L power:VCC #PWR025
U 1 1 5BB02951
P 3100 4200
F 0 "#PWR025" H 3100 4050 50  0001 C CNN
F 1 "VCC" H 3117 4373 50  0000 C CNN
F 2 "" H 3100 4200 50  0001 C CNN
F 3 "" H 3100 4200 50  0001 C CNN
	1    3100 4200
	1    0    0    -1  
$EndComp
$Comp
L freetronics_schematic:CAP C12
U 1 1 5BB09AC6
P 4200 4800
F 0 "C12" H 4335 4846 50  0000 L CNN
F 1 "1μF" H 4335 4755 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 4200 4800 60  0001 C CNN
F 3 "" H 4200 4800 60  0000 C CNN
	1    4200 4800
	1    0    0    -1  
$EndComp
$Comp
L freetronics_schematic:CAP C13
U 1 1 5BB09B2B
P 4550 4800
F 0 "C13" H 4685 4846 50  0000 L CNN
F 1 "100nF" H 4685 4755 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 4550 4800 60  0001 C CNN
F 3 "" H 4550 4800 60  0000 C CNN
	1    4550 4800
	1    0    0    -1  
$EndComp
$Comp
L freetronics_schematic:CAP C11
U 1 1 5BB09BFE
P 3900 4800
F 0 "C11" H 4035 4846 50  0000 L CNN
F 1 "10μF" H 4035 4755 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 3900 4800 60  0001 C CNN
F 3 "" H 3900 4800 60  0000 C CNN
	1    3900 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 4600 3900 4450
Connection ~ 3900 4450
Wire Wire Line
	3900 4450 3200 4450
Wire Wire Line
	3900 5000 3900 5100
Connection ~ 3900 5100
Wire Wire Line
	3900 5100 4200 5100
Wire Wire Line
	3900 4450 4200 4450
Wire Wire Line
	4550 4600 4550 4450
Connection ~ 4550 4450
Wire Wire Line
	4550 5000 4550 5100
Connection ~ 4550 5100
Wire Wire Line
	4550 5100 5400 5100
Wire Wire Line
	4200 5000 4200 5100
Connection ~ 4200 5100
Wire Wire Line
	4200 5100 4550 5100
Wire Wire Line
	4200 4600 4200 4450
Connection ~ 4200 4450
Wire Wire Line
	4200 4450 4550 4450
$Comp
L DMPro17-rescue:GND #PWR028
U 1 1 5BBA3CEF
P 5650 4250
F 0 "#PWR028" H 5650 4000 50  0001 C CNN
F 1 "GND" V 5655 4122 50  0000 R CNN
F 2 "" H 5650 4250 50  0001 C CNN
F 3 "" H 5650 4250 50  0001 C CNN
	1    5650 4250
	0    1    1    0   
$EndComp
$Comp
L Device:Jumper_NC_Dual JP10
U 1 1 5BB1B928
P 4800 4150
F 0 "JP10" V 4846 4252 50  0000 L CNN
F 1 "Jumper_NC_Dual" V 4755 4252 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 4800 4150 50  0001 C CNN
F 3 "~" H 4800 4150 50  0001 C CNN
	1    4800 4150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4550 4450 4800 4450
Wire Wire Line
	4800 4450 4800 4400
Connection ~ 4800 4450
Wire Wire Line
	4800 4450 5650 4450
Wire Wire Line
	4900 4150 5650 4150
$Comp
L DMPro17-rescue:VCC #PWR0101
U 1 1 5BB1BBE0
P 4800 3900
F 0 "#PWR0101" H 4800 3750 50  0001 C CNN
F 1 "VCC" H 4817 4073 50  0000 C CNN
F 2 "" H 4800 3900 50  0001 C CNN
F 3 "" H 4800 3900 50  0001 C CNN
	1    4800 3900
	1    0    0    -1  
$EndComp
$EndSCHEMATC
