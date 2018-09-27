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
Wire Wire Line
	5300 3850 5400 3850
Wire Wire Line
	5300 3950 5400 3950
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
$Comp
L Device:Battery BT?
U 1 1 5BB02470
P 3100 4800
AR Path="/5BB02470" Ref="BT?"  Part="1" 
AR Path="/5BB1F7DF/5BB02470" Ref="BT1"  Part="1" 
F 0 "BT1" H 3208 4846 50  0000 L CNN
F 1 "Battery" H 3208 4755 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" V 3100 4860 50  0001 C CNN
F 3 "~" V 3100 4860 50  0001 C CNN
	1    3100 4800
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NC_Dual JP10
U 1 1 5BB0256D
P 3100 4350
F 0 "JP10" V 3146 4452 50  0000 L CNN
F 1 "Jumper_NC_Dual" V 3055 4452 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3100 4350 50  0001 C CNN
F 3 "~" H 3100 4350 50  0001 C CNN
	1    3100 4350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3100 5000 3900 5000
Wire Wire Line
	5400 5000 5400 4550
Connection ~ 5400 4550
$Comp
L power:VCC #PWR0102
U 1 1 5BB02951
P 3100 4100
F 0 "#PWR0102" H 3100 3950 50  0001 C CNN
F 1 "VCC" H 3117 4273 50  0000 C CNN
F 2 "" H 3100 4100 50  0001 C CNN
F 3 "" H 3100 4100 50  0001 C CNN
	1    3100 4100
	1    0    0    -1  
$EndComp
$Comp
L freetronics_schematic:CAP C12
U 1 1 5BB09AC6
P 4200 4700
F 0 "C12" H 4335 4746 50  0000 L CNN
F 1 "1μF" H 4335 4655 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 4200 4700 60  0001 C CNN
F 3 "" H 4200 4700 60  0000 C CNN
	1    4200 4700
	1    0    0    -1  
$EndComp
$Comp
L freetronics_schematic:CAP C13
U 1 1 5BB09B2B
P 4550 4700
F 0 "C13" H 4685 4746 50  0000 L CNN
F 1 "100nF" H 4685 4655 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 4550 4700 60  0001 C CNN
F 3 "" H 4550 4700 60  0000 C CNN
	1    4550 4700
	1    0    0    -1  
$EndComp
$Comp
L freetronics_schematic:CAP C11
U 1 1 5BB09BFE
P 3900 4700
F 0 "C11" H 4035 4746 50  0000 L CNN
F 1 "10μF" H 4035 4655 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 3900 4700 60  0001 C CNN
F 3 "" H 3900 4700 60  0000 C CNN
	1    3900 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 4500 3900 4350
Connection ~ 3900 4350
Wire Wire Line
	3900 4350 3200 4350
Wire Wire Line
	3900 4900 3900 5000
Connection ~ 3900 5000
Wire Wire Line
	3900 5000 4200 5000
Wire Wire Line
	3900 4350 4200 4350
Wire Wire Line
	4550 4500 4550 4350
Connection ~ 4550 4350
Wire Wire Line
	4550 4350 5400 4350
Wire Wire Line
	4550 4900 4550 5000
Connection ~ 4550 5000
Wire Wire Line
	4550 5000 5400 5000
Wire Wire Line
	4200 4900 4200 5000
Connection ~ 4200 5000
Wire Wire Line
	4200 5000 4550 5000
Wire Wire Line
	4200 4500 4200 4350
Connection ~ 4200 4350
Wire Wire Line
	4200 4350 4550 4350
$EndSCHEMATC