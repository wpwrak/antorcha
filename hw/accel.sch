EESchema Schematic File Version 2  date Thu Jun 14 14:45:51 2012
LIBS:c
LIBS:r
LIBS:led
LIBS:Atmega8
LIBS:at86rf231
LIBS:balun-smt6
LIBS:antenna
LIBS:sw4
LIBS:device_sot
LIBS:xtal-4
LIBS:adxl32x
LIBS:gencon
LIBS:pwr
LIBS:powered
LIBS:testpoint
LIBS:antorcha-cache
EELAYER 25  0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 7 7
Title ""
Date "14 jun 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L C C803
U 1 1 4FD2B1FA
P 7200 4700
F 0 "C803" H 7250 4800 60  0000 L CNN
F 1 "100n" H 7250 4600 60  0000 L CNN
	1    7200 4700
	1    0    0    -1  
$EndComp
$Comp
L C C802
U 1 1 4FD2B209
P 6700 4700
F 0 "C802" H 6750 4800 60  0000 L CNN
F 1 "100n" H 6750 4600 60  0000 L CNN
	1    6700 4700
	1    0    0    -1  
$EndComp
$Comp
L C C801
U 1 1 4FD2B218
P 4800 3400
F 0 "C801" H 4850 3500 60  0000 L CNN
F 1 "100n" H 4850 3300 60  0000 L CNN
	1    4800 3400
	1    0    0    -1  
$EndComp
Text GLabel 7600 4000 2    60   Output ~ 0
X
Text GLabel 7600 4200 2    60   Output ~ 0
Y
$Comp
L GND #PWR031
U 1 1 4FD4B9B6
P 6700 5250
F 0 "#PWR031" H 6700 5250 30  0001 C CNN
F 1 "GND" H 6700 5180 30  0001 C CNN
	1    6700 5250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR032
U 1 1 4FD4B9C5
P 7200 5250
F 0 "#PWR032" H 7200 5250 30  0001 C CNN
F 1 "GND" H 7200 5180 30  0001 C CNN
	1    7200 5250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR033
U 1 1 4FD4B9D4
P 5600 5200
F 0 "#PWR033" H 5600 5200 30  0001 C CNN
F 1 "GND" H 5600 5130 30  0001 C CNN
	1    5600 5200
	1    0    0    -1  
$EndComp
Text GLabel 7600 3000 2    60   Input ~ 0
CTRL_4V5
$Comp
L GND #PWR034
U 1 1 4FD4BAE5
P 4800 4350
F 0 "#PWR034" H 4800 4350 30  0001 C CNN
F 1 "GND" H 4800 4280 30  0001 C CNN
	1    4800 4350
	1    0    0    -1  
$EndComp
NoConn ~ 5000 4100
$Comp
L ADXL32X U801
U 1 1 4FD5E75E
P 5750 4150
F 0 "U801" H 5350 4650 60  0000 C CNN
F 1 "ADXL32X" H 5750 4150 60  0000 C CNN
	1    5750 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 4000 7600 4000
Wire Wire Line
	6500 4200 7600 4200
Wire Wire Line
	5600 4900 5600 5200
Wire Wire Line
	5600 5050 6100 5050
Wire Wire Line
	5800 5050 5800 4900
Connection ~ 5600 5050
Wire Wire Line
	5700 4900 5700 5050
Connection ~ 5700 5050
Wire Wire Line
	6700 4900 6700 5250
Wire Wire Line
	7200 4900 7200 5250
Wire Wire Line
	6700 4500 6700 4000
Connection ~ 6700 4000
Wire Wire Line
	7200 4500 7200 4200
Connection ~ 7200 4200
Wire Wire Line
	4800 3000 7600 3000
Wire Wire Line
	5700 3000 5700 3400
Wire Wire Line
	5800 3400 5800 3000
Connection ~ 5800 3000
Wire Wire Line
	4800 4350 4800 3600
Connection ~ 5700 3000
Wire Wire Line
	5000 4200 4800 4200
Connection ~ 4800 4200
Wire Wire Line
	6100 5050 6100 4900
Connection ~ 5800 5050
Wire Wire Line
	4800 3000 4800 3200
$EndSCHEMATC
