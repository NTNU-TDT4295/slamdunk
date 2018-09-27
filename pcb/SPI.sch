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
L Connector_Generic:Conn_02x03_Counter_Clockwise J?
U 1 1 5BAD9D96
P 6850 4050
AR Path="/5BAD9D96" Ref="J?"  Part="1" 
AR Path="/5BAD9CF1/5BAD9D96" Ref="J2"  Part="1" 
F 0 "J2" V 6854 4230 50  0000 L CNN
F 1 "Conn_02x03_Counter_Clockwise" V 6945 4230 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x03_P2.54mm_Vertical" H 6850 4050 50  0001 C CNN
F 3 "~" H 6850 4050 50  0001 C CNN
	1    6850 4050
	0    -1   -1   0   
$EndComp
Text HLabel 6950 4350 3    50   Input ~ 0
MOSI
Text HLabel 6750 3650 1    50   Input ~ 0
MISO
Text HLabel 6950 3650 1    50   Input ~ 0
SCLK
Text HLabel 6850 4350 3    50   Input ~ 0
SS
Wire Wire Line
	6850 4350 6850 4250
Wire Wire Line
	6950 3650 6950 3750
Wire Wire Line
	6750 3650 6750 3750
Wire Wire Line
	6950 4350 6950 4250
$EndSCHEMATC
