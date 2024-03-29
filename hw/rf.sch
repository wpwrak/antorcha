EESchema Schematic File Version 2  date Sat Jun 16 10:25:00 2012
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
Sheet 6 7
Title ""
Date "16 jun 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L AT86RF231 U701
U 1 1 4FD2B227
P 7200 3800
F 0 "U701" H 6200 5000 60  0000 C CNN
F 1 "AT86RF231" H 7200 3800 60  0000 C CNN
	1    7200 3800
	1    0    0    -1  
$EndComp
$Comp
L BALUN-SMT6 B701
U 1 1 4FD2B236
P 3400 3900
F 0 "B701" H 3100 4250 60  0000 C CNN
F 1 "BALUN-SMT6" H 3300 3450 60  0000 C CNN
	1    3400 3900
	1    0    0    -1  
$EndComp
$Comp
L ANTENNA ANT701
U 1 1 4FD2B245
P 1800 3750
F 0 "ANT701" H 1800 4000 60  0000 C CNN
F 1 "50R" H 1800 3500 60  0000 C CNN
	1    1800 3750
	1    0    0    -1  
$EndComp
$Comp
L XTAL-4 X701
U 1 1 4FD2B8C7
P 8900 1550
F 0 "X701" H 8900 1900 60  0000 C CNN
F 1 "XTAL-4" H 8900 1800 60  0000 C CNN
	1    8900 1550
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR012
U 1 1 4FD51684
P 6600 5600
F 0 "#PWR012" H 6600 5600 30  0001 C CNN
F 1 "GND" H 6600 5530 30  0001 C CNN
	1    6600 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 4FD516AA
P 7050 5600
F 0 "#PWR013" H 7050 5600 30  0001 C CNN
F 1 "GND" H 7050 5530 30  0001 C CNN
	1    7050 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 4FD516B0
P 7650 5600
F 0 "#PWR014" H 7650 5600 30  0001 C CNN
F 1 "GND" H 7650 5530 30  0001 C CNN
	1    7650 5600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 4FD516B6
P 7200 6800
F 0 "#PWR015" H 7200 6800 30  0001 C CNN
F 1 "GND" H 7200 6730 30  0001 C CNN
	1    7200 6800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR016
U 1 1 4FD516F6
P 8800 4600
F 0 "#PWR016" H 8800 4600 30  0001 C CNN
F 1 "GND" H 8800 4530 30  0001 C CNN
	1    8800 4600
	1    0    0    -1  
$EndComp
$Comp
L TESTPOINT TP601
U 1 1 4FD51723
P 9300 4400
F 0 "TP601" H 9300 4650 60  0000 C CNN
F 1 "TESTPOINT" H 9300 4350 60  0001 C CNN
	1    9300 4400
	0    1    1    0   
$EndComp
Text GLabel 10650 4100 2    60   Input ~ 0
RF_SCLK
Text GLabel 10650 3950 2    60   Output ~ 0
RF_MISO
Text GLabel 10650 3650 2    60   Input ~ 0
RF_MOSI
Text GLabel 10650 3500 2    60   Input ~ 0
nSS
$Comp
L C C601
U 1 1 4FD517BE
P 9300 900
F 0 "C601" H 9350 1000 60  0000 L CNN
F 1 "12p" H 9350 800 60  0000 L CNN
	1    9300 900 
	0    -1   -1   0   
$EndComp
$Comp
L C C604
U 1 1 4FD517CD
P 9300 2200
F 0 "C604" H 9350 2300 60  0000 L CNN
F 1 "12p" H 9350 2100 60  0000 L CNN
	1    9300 2200
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR017
U 1 1 4FD5181F
P 9800 2400
F 0 "#PWR017" H 9800 2400 30  0001 C CNN
F 1 "GND" H 9800 2330 30  0001 C CNN
	1    9800 2400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 4FD519CA
P 5650 4600
F 0 "#PWR018" H 5650 4600 30  0001 C CNN
F 1 "GND" H 5650 4530 30  0001 C CNN
	1    5650 4600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR019
U 1 1 4FD519D0
P 7350 2200
F 0 "#PWR019" H 7350 2200 30  0001 C CNN
F 1 "GND" H 7350 2130 30  0001 C CNN
	1    7350 2200
	-1   0    0    1   
$EndComp
Text GLabel 5250 4400 0    60   Input ~ 0
RF_nRST
$Comp
L C C603
U 1 1 4FD51D41
P 6700 1400
F 0 "C603" H 6750 1500 60  0000 L CNN
F 1 "1u" H 6750 1300 60  0000 L CNN
	1    6700 1400
	1    0    0    -1  
$EndComp
$Comp
L C C602
U 1 1 4FD51D4E
P 6300 1400
F 0 "C602" H 6350 1500 60  0000 L CNN
F 1 "1u" H 6350 1300 60  0000 L CNN
	1    6300 1400
	1    0    0    -1  
$EndComp
$Comp
L C C608
U 1 1 4FD51D54
P 7200 6400
F 0 "C608" H 7250 6500 60  0000 L CNN
F 1 "1u" H 7250 6300 60  0000 L CNN
	1    7200 6400
	1    0    0    -1  
$EndComp
$Comp
L C C609
U 1 1 4FD51D5A
P 7500 6400
F 0 "C609" H 7550 6500 60  0000 L CNN
F 1 "1u" H 7550 6300 60  0000 L CNN
	1    7500 6400
	1    0    0    -1  
$EndComp
$Comp
L C C606
U 1 1 4FD51D60
P 4600 4100
F 0 "C606" V 4700 4200 60  0000 L CNN
F 1 "22pF/RF" V 4700 3600 60  0000 L CNN
	1    4600 4100
	0    -1   -1   0   
$EndComp
$Comp
L C C605
U 1 1 4FD51D66
P 4600 3700
F 0 "C605" V 4700 3800 60  0000 L CNN
F 1 "22pF/RF" V 4700 3200 60  0000 L CNN
	1    4600 3700
	0    -1   -1   0   
$EndComp
$Comp
L C C607
U 1 1 4FD51D6C
P 4000 4500
F 0 "C607" H 4050 4600 60  0000 L CNN
F 1 "22pF/RF" H 4050 4400 60  0000 L CNN
	1    4000 4500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR020
U 1 1 4FD51F66
P 7500 6800
F 0 "#PWR020" H 7500 6800 30  0001 C CNN
F 1 "GND" H 7500 6730 30  0001 C CNN
	1    7500 6800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR021
U 1 1 4FD522E8
P 4000 4900
F 0 "#PWR021" H 4000 4900 30  0001 C CNN
F 1 "GND" H 4000 4830 30  0001 C CNN
	1    4000 4900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR022
U 1 1 4FD522EE
P 2800 4900
F 0 "#PWR022" H 2800 4900 30  0001 C CNN
F 1 "GND" H 2800 4830 30  0001 C CNN
	1    2800 4900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR023
U 1 1 4FD524AF
P 2300 4900
F 0 "#PWR023" H 2300 4900 30  0001 C CNN
F 1 "GND" H 2300 4830 30  0001 C CNN
	1    2300 4900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR024
U 1 1 4FD526B0
P 6300 1800
F 0 "#PWR024" H 6300 1800 30  0001 C CNN
F 1 "GND" H 6300 1730 30  0001 C CNN
	1    6300 1800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 4FD526B6
P 6700 1800
F 0 "#PWR025" H 6700 1800 30  0001 C CNN
F 1 "GND" H 6700 1730 30  0001 C CNN
	1    6700 1800
	1    0    0    -1  
$EndComp
Text GLabel 6000 800  0    60   Input ~ 0
CTRL_3V
Text GLabel 9200 6000 2    60   Input ~ 0
CTRL_3V
$Comp
L GND #PWR026
U 1 1 4FD52A00
P 8100 5600
F 0 "#PWR026" H 8100 5600 30  0001 C CNN
F 1 "GND" H 8100 5530 30  0001 C CNN
	1    8100 5600
	1    0    0    -1  
$EndComp
Text Label 9000 4400 0    60   ~ 0
CLKM
Text GLabel 10650 3300 2    60   Output ~ 0
RF_IRQ
$Comp
L POWERED #FLG027
U 1 1 4FD53072
P 8800 5600
F 0 "#FLG027" H 9000 5500 60  0001 C CNN
F 1 "POWERED" H 8800 5650 60  0000 C CNN
	1    8800 5600
	1    0    0    -1  
$EndComp
$Comp
L POWERED #FLG028
U 1 1 4FD53220
P 4000 3300
F 0 "#FLG028" H 4200 3200 60  0001 C CNN
F 1 "POWERED" H 4000 3350 60  0000 C CNN
	1    4000 3300
	1    0    0    -1  
$EndComp
$Comp
L R R602
U 1 1 4FD6959A
P 9700 4850
F 0 "R602" H 9900 4900 60  0000 C CNN
F 1 "22k" H 9850 4800 60  0000 C CNN
	1    9700 4850
	1    0    0    -1  
$EndComp
$Comp
L R R603
U 1 1 4FD695A7
P 10100 4850
F 0 "R603" H 10300 4900 60  0000 C CNN
F 1 "22k" H 10250 4800 60  0000 C CNN
	1    10100 4850
	1    0    0    -1  
$EndComp
$Comp
L R R604
U 1 1 4FD695C1
P 10500 4850
F 0 "R604" H 10700 4900 60  0000 C CNN
F 1 "22k" H 10650 4800 60  0000 C CNN
	1    10500 4850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR029
U 1 1 4FD695D1
P 9700 5500
F 0 "#PWR029" H 9700 5500 30  0001 C CNN
F 1 "GND" H 9700 5430 30  0001 C CNN
	1    9700 5500
	1    0    0    -1  
$EndComp
$Comp
L R R605
U 1 1 4FD69C35
P 5400 5050
F 0 "R605" H 5600 5100 60  0000 C CNN
F 1 "22k" H 5550 5000 60  0000 C CNN
	1    5400 5050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR030
U 1 1 4FD69C3B
P 5400 5500
F 0 "#PWR030" H 5400 5500 30  0001 C CNN
F 1 "GND" H 5400 5430 30  0001 C CNN
	1    5400 5500
	1    0    0    -1  
$EndComp
Text Label 2400 3700 0    60   ~ 0
FEED
$Comp
L R R601
U 1 1 4FDC8D53
P 6900 6350
F 0 "R601" H 7100 6400 60  0000 C CNN
F 1 "10k" H 7050 6300 60  0000 C CNN
	1    6900 6350
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR031
U 1 1 4FDC8D6F
P 6900 6800
F 0 "#PWR031" H 6900 6800 30  0001 C CNN
F 1 "GND" H 6900 6730 30  0001 C CNN
	1    6900 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6600 5200 6600 5600
Wire Wire Line
	6750 5200 6750 5400
Wire Wire Line
	6750 5400 6600 5400
Connection ~ 6600 5400
Wire Wire Line
	7050 5200 7050 5600
Wire Wire Line
	7650 5200 7650 5600
Wire Wire Line
	8100 5200 8100 5600
Wire Wire Line
	8600 3800 8800 3800
Wire Wire Line
	8800 3800 8800 4600
Wire Wire Line
	8600 4250 8800 4250
Connection ~ 8800 4250
Wire Wire Line
	8600 4400 9300 4400
Wire Wire Line
	8600 3500 10650 3500
Wire Wire Line
	8600 3650 10650 3650
Wire Wire Line
	8600 3950 10650 3950
Wire Wire Line
	8600 4100 10650 4100
Wire Wire Line
	9500 900  9800 900 
Wire Wire Line
	9800 900  9800 2400
Wire Wire Line
	9500 2200 9800 2200
Connection ~ 9800 2200
Wire Wire Line
	9300 1400 9800 1400
Connection ~ 9800 1400
Wire Wire Line
	9300 1700 9800 1700
Connection ~ 9800 1700
Wire Wire Line
	8900 1100 8900 900 
Wire Wire Line
	7500 900  9100 900 
Wire Wire Line
	8900 2000 8900 2200
Wire Wire Line
	7650 2200 9100 2200
Wire Wire Line
	7650 2400 7650 2200
Connection ~ 8900 2200
Wire Wire Line
	7500 2400 7500 900 
Connection ~ 8900 900 
Wire Wire Line
	7350 2200 7350 2400
Wire Wire Line
	5650 2200 5650 4600
Wire Wire Line
	5650 3350 5800 3350
Wire Wire Line
	5650 3500 5800 3500
Connection ~ 5650 3500
Connection ~ 5650 3500
Wire Wire Line
	5800 3650 5650 3650
Connection ~ 5650 3650
Wire Wire Line
	5800 4100 5650 4100
Connection ~ 5650 4100
Wire Wire Line
	5800 4250 5650 4250
Connection ~ 5650 4250
Wire Wire Line
	5250 4400 5800 4400
Wire Wire Line
	5650 2200 6900 2200
Wire Wire Line
	6900 2200 6900 2400
Connection ~ 5650 3350
Wire Wire Line
	6750 2400 6750 2200
Connection ~ 6750 2200
Wire Wire Line
	6600 2200 6600 2400
Connection ~ 6600 2200
Wire Wire Line
	7200 5200 7200 6200
Wire Wire Line
	7350 5200 7350 5400
Wire Wire Line
	7350 5400 7200 5400
Connection ~ 7200 5400
Wire Wire Line
	7200 6600 7200 6800
Wire Wire Line
	7500 5200 7500 6200
Wire Wire Line
	7500 6600 7500 6800
Wire Wire Line
	3800 3900 4000 3900
Wire Wire Line
	4000 3500 4000 4300
Wire Wire Line
	3800 3700 4400 3700
Wire Wire Line
	3800 4100 4400 4100
Wire Wire Line
	4800 4100 5200 4100
Wire Wire Line
	5200 4100 5200 3800
Wire Wire Line
	5200 3800 5800 3800
Wire Wire Line
	4800 3700 5400 3700
Wire Wire Line
	5400 3700 5400 3950
Wire Wire Line
	5400 3950 5800 3950
Wire Wire Line
	2800 4100 2800 4900
Wire Wire Line
	2800 4100 3000 4100
Wire Wire Line
	3000 4200 2800 4200
Connection ~ 2800 4200
Wire Wire Line
	4000 4700 4000 4900
Wire Wire Line
	2100 3700 3000 3700
Wire Wire Line
	2100 3800 2300 3800
Wire Wire Line
	2300 3800 2300 4900
Wire Wire Line
	6300 1600 6300 1800
Wire Wire Line
	6700 1600 6700 1800
Wire Wire Line
	6700 1200 6700 1000
Wire Wire Line
	6700 1000 7050 1000
Wire Wire Line
	7050 1000 7050 2400
Wire Wire Line
	6300 1200 6300 800 
Wire Wire Line
	6000 800  7200 800 
Wire Wire Line
	7200 800  7200 2400
Connection ~ 6300 800 
Wire Wire Line
	7500 6000 9200 6000
Connection ~ 7500 6000
Wire Wire Line
	6900 5200 6900 6100
Wire Wire Line
	8800 5800 8800 6000
Connection ~ 8800 6000
Connection ~ 4000 3900
Wire Wire Line
	8600 3350 8800 3350
Wire Wire Line
	8800 3350 8800 3300
Wire Wire Line
	8800 3300 10650 3300
Wire Wire Line
	9700 4600 9700 3500
Connection ~ 9700 3500
Wire Wire Line
	10100 4600 10100 3650
Connection ~ 10100 3650
Wire Wire Line
	10500 4600 10500 4100
Connection ~ 10500 4100
Wire Wire Line
	9700 5100 9700 5500
Wire Wire Line
	9700 5300 10500 5300
Wire Wire Line
	10500 5300 10500 5100
Connection ~ 9700 5300
Wire Wire Line
	10100 5100 10100 5300
Connection ~ 10100 5300
Wire Wire Line
	5400 4400 5400 4800
Connection ~ 5400 4400
Connection ~ 5400 4400
Wire Wire Line
	5400 5500 5400 5300
Wire Wire Line
	6900 6600 6900 6800
$EndSCHEMATC
