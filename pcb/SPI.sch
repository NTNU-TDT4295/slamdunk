EESchema Schematic File Version 4
LIBS:pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 9
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
P 5550 3650
AR Path="/5BAD9D96" Ref="J?"  Part="1" 
AR Path="/5BAD9CF1/5BAD9D96" Ref="J2"  Part="1" 
F 0 "J2" V 5554 3830 50  0000 L CNN
F 1 "Conn_02x03_Counter_Clockwise" V 5645 3830 50  0000 L CNN
F 2 "" H 5550 3650 50  0001 C CNN
F 3 "~" H 5550 3650 50  0001 C CNN
	1    5550 3650
	0    1    1    0   
$EndComp
Text HLabel 5550 4050 3    50   Input ~ 0
MOSI
Text HLabel 5650 3350 1    50   Input ~ 0
MISO
Text HLabel 5550 3350 1    50   Input ~ 0
SCLK
Text HLabel 5450 3350 1    50   Input ~ 0
SS
Wire Wire Line
	5450 3350 5450 3450
Wire Wire Line
	5550 3350 5550 3450
Wire Wire Line
	5650 3350 5650 3450
Wire Wire Line
	5550 4050 5550 3950
$EndSCHEMATC
