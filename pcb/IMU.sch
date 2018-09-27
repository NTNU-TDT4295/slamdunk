EESchema Schematic File Version 4
LIBS:pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 7 10
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
L localization-symbols:IMU U?
U 1 1 5BB1BB38
P 6000 3950
AR Path="/5BB1BB38" Ref="U?"  Part="1" 
AR Path="/5BB1BAF2/5BB1BB38" Ref="U3"  Part="1" 
F 0 "U3" V 6021 3622 50  0000 R CNN
F 1 "IMU" V 5930 3622 50  0000 R CNN
F 2 "Adafruit BNO055:BNO055" V 6150 3850 50  0001 C CNN
F 3 "" V 6150 3850 50  0001 C CNN
	1    6000 3950
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR023
U 1 1 5BB1BBFA
P 6050 3550
F 0 "#PWR023" H 6050 3300 50  0001 C CNN
F 1 "GND" H 6055 3377 50  0000 C CNN
F 2 "" H 6050 3550 50  0001 C CNN
F 3 "" H 6050 3550 50  0001 C CNN
	1    6050 3550
	-1   0    0    1   
$EndComp
Text HLabel 5750 3550 1    50   Input ~ 0
RST
Text HLabel 5850 3550 1    50   Input ~ 0
SCL
Text HLabel 5950 3550 1    50   Input ~ 0
SDA
Wire Wire Line
	5750 3550 5750 3650
Wire Wire Line
	5850 3550 5850 3650
Wire Wire Line
	5950 3550 5950 3650
Wire Wire Line
	6050 3550 6050 3650
NoConn ~ 6150 3650
NoConn ~ 6150 4300
NoConn ~ 6050 4300
NoConn ~ 5950 4300
NoConn ~ 5850 4300
$Comp
L power:+3.3V #PWR0101
U 1 1 5BAD3F69
P 6250 3550
F 0 "#PWR0101" H 6250 3400 50  0001 C CNN
F 1 "+3.3V" H 6265 3723 50  0000 C CNN
F 2 "" H 6250 3550 50  0001 C CNN
F 3 "" H 6250 3550 50  0001 C CNN
	1    6250 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 3650 6250 3550
$EndSCHEMATC
