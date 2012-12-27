EESchema Schematic File Version 2  date Wed Dec 26 21:38:03 2012
LIBS:c
LIBS:r
LIBS:inductor
LIBS:led
LIBS:con
LIBS:lpc1100-qfn33
LIBS:lis3dh
LIBS:gencon
LIBS:pwr
LIBS:powered
LIBS:evqq7
LIBS:pmosfet-gsd
LIBS:tps6216x
LIBS:memcard8
EELAYER 25  0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date "27 dec 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L PMOSFET-GSD Q1
U 1 1 50DB9455
P 2250 4500
F 0 "Q1" H 2100 4825 60  0000 C CNN
F 1 "PMOSFET-GSD" H 1900 4200 60  0000 C CNN
	1    2250 4500
	1    0    0    1   
$EndComp
$Comp
L GND #PWR021
U 1 1 50DB9461
P 2950 6100
F 0 "#PWR021" H 2950 6100 30  0001 C CNN
F 1 "GND" H 2950 6030 30  0001 C CNN
	1    2950 6100
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 50DB9467
P 2950 4500
F 0 "C7" H 3000 4600 60  0000 L CNN
F 1 "1u" H 3000 4400 60  0000 L CNN
	1    2950 4500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR022
U 1 1 50DB9474
P 7500 4000
F 0 "#PWR022" H 7500 4000 30  0001 C CNN
F 1 "GND" H 7500 3930 30  0001 C CNN
	1    7500 4000
	1    0    0    -1  
$EndComp
Text Label 2450 5500 0    60   ~ 0
VCARD
$Comp
L LIS3DH U3
U 1 1 50DB94A4
P 8600 3500
F 0 "U3" H 8000 4150 60  0000 L CNN
F 1 "LIS3DH" H 8600 3650 60  0000 C CNN
	1    8600 3500
	1    0    0    -1  
$EndComp
$Comp
L MEMCARD8-SHIELD4-SW CON5
U 1 1 50DB94B0
P 3500 5600
F 0 "CON5" H 3400 6100 60  0000 L CNN
F 1 "MEMCARD8-SHIELD4-SW" H 3750 5150 60  0000 C CNN
	1    3500 5600
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 50DB94C8
P 6700 2800
F 0 "C5" H 6750 2900 60  0000 L CNN
F 1 "10u" H 6750 2700 60  0000 L CNN
	1    6700 2800
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 50DB94DA
P 7100 2800
F 0 "C6" H 7150 2900 60  0000 L CNN
F 1 "100n" H 7150 2700 60  0000 L CNN
	1    7100 2800
	1    0    0    -1  
$EndComp
NoConn ~ 8550 2600
NoConn ~ 8700 2600
NoConn ~ 9500 3200
$Comp
L GND #PWR023
U 1 1 50DB9508
P 4600 6100
F 0 "#PWR023" H 4600 6100 30  0001 C CNN
F 1 "GND" H 4600 6030 30  0001 C CNN
	1    4600 6100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR024
U 1 1 50DB9525
P 7100 3200
F 0 "#PWR024" H 7100 3200 30  0001 C CNN
F 1 "GND" H 7100 3130 30  0001 C CNN
	1    7100 3200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 50DB952B
P 6700 3200
F 0 "#PWR025" H 6700 3200 30  0001 C CNN
F 1 "GND" H 6700 3130 30  0001 C CNN
	1    6700 3200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR026
U 1 1 50DB9531
P 9700 4000
F 0 "#PWR026" H 9700 4000 30  0001 C CNN
F 1 "GND" H 9700 3930 30  0001 C CNN
	1    9700 4000
	1    0    0    -1  
$EndComp
NoConn ~ 9500 3800
$Comp
L 3V3 #PWR027
U 1 1 50DB955F
P 2950 3700
F 0 "#PWR027" H 2950 3660 30  0001 C CNN
F 1 "3V3" H 2950 3850 60  0000 C CNN
	1    2950 3700
	1    0    0    -1  
$EndComp
$Comp
L 3V3 #PWR028
U 1 1 50DB958F
P 8850 2200
F 0 "#PWR028" H 8850 2160 30  0001 C CNN
F 1 "3V3" H 8850 2350 60  0000 C CNN
	1    8850 2200
	1    0    0    -1  
$EndComp
NoConn ~ 4400 5600
NoConn ~ 4400 5800
Text GLabel 9900 3500 2    60   Output ~ 0
ACC_INT
Text GLabel 7450 3650 0    60   Input ~ 0
ACC_SCLK
Text GLabel 8550 4600 3    60   Input ~ 0
ACC_MOSI
Text GLabel 8700 4600 3    60   Output ~ 0
ACC_MISO
Text GLabel 8850 4600 3    60   Input ~ 0
ACC_nCS
Text GLabel 1700 5200 0    60   BiDi ~ 0
DAT2
Text GLabel 1700 5300 0    60   BiDi ~ 0
DAT3
Text GLabel 1700 5400 0    60   BiDi ~ 0
CMD
Text GLabel 1700 5600 0    60   Input ~ 0
CLK
Text GLabel 1700 5800 0    60   BiDi ~ 0
DAT0
Text GLabel 1700 5900 0    60   BiDi ~ 0
DAT1
Text GLabel 1700 4500 0    60   Input ~ 0
nPWR
Text GLabel 4800 5200 2    60   Output ~ 0
CARD_SW
Wire Wire Line
	1700 5400 3150 5400
Wire Wire Line
	1700 5800 3150 5800
Wire Wire Line
	1700 5900 3150 5900
Wire Wire Line
	3150 5700 2950 5700
Wire Wire Line
	2950 4700 2950 6100
Wire Wire Line
	3150 5500 2350 5500
Wire Wire Line
	2350 5500 2350 4900
Connection ~ 2950 5700
Wire Wire Line
	2350 4100 2350 3900
Wire Wire Line
	2950 3700 2950 4300
Connection ~ 2950 3900
Wire Wire Line
	1700 5600 3150 5600
Wire Wire Line
	1700 5200 3150 5200
Wire Wire Line
	1700 4500 1950 4500
Wire Wire Line
	3150 5300 1700 5300
Wire Wire Line
	4600 5300 4600 6100
Wire Wire Line
	4400 5700 4600 5700
Connection ~ 4600 5700
Wire Wire Line
	4400 5900 4600 5900
Connection ~ 4600 5900
Wire Wire Line
	7700 3200 7500 3200
Wire Wire Line
	7500 2400 7500 3200
Wire Wire Line
	6700 2400 8850 2400
Wire Wire Line
	8850 2200 8850 2600
Wire Wire Line
	7100 2600 7100 2400
Connection ~ 7500 2400
Wire Wire Line
	6700 2600 6700 2400
Connection ~ 7100 2400
Wire Wire Line
	6700 3000 6700 3200
Wire Wire Line
	7100 3000 7100 3200
Wire Wire Line
	7700 3800 7500 3800
Wire Wire Line
	7500 3800 7500 4000
Wire Wire Line
	9500 3350 9700 3350
Wire Wire Line
	9700 3350 9700 4000
Wire Wire Line
	9500 3650 9700 3650
Connection ~ 9700 3650
Wire Wire Line
	2350 3900 2950 3900
Connection ~ 8850 2400
Wire Wire Line
	4400 5300 4600 5300
Wire Wire Line
	4400 5200 4800 5200
Wire Wire Line
	7450 3650 7700 3650
Wire Wire Line
	9500 3500 9900 3500
Wire Wire Line
	8550 4400 8550 4600
Wire Wire Line
	8700 4400 8700 4600
Wire Wire Line
	8850 4400 8850 4600
$EndSCHEMATC
