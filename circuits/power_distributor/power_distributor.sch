EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:power_distributor-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Modular Synthesizer Power Distributor"
Date "2017-07-12"
Rev ""
Comp "Alexandros Drymonitis"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X02 P1
U 1 1 5965D762
P 6450 3400
F 0 "P1" H 6450 3550 50  0000 C CNN
F 1 "CONN_01X02" V 6550 3400 50  0000 C CNN
F 2 "Connect:bornier2" H 6450 3400 50  0001 C CNN
F 3 "" H 6450 3400 50  0000 C CNN
	1    6450 3400
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P2
U 1 1 5965D7FC
P 7550 2450
F 0 "P2" H 7550 2600 50  0000 C CNN
F 1 "CONN_01X02" V 7650 2450 50  0000 C CNN
F 2 "Connect:bornier2" H 7550 2450 50  0001 C CNN
F 3 "" H 7550 2450 50  0000 C CNN
	1    7550 2450
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P3
U 1 1 5965D835
P 7550 3100
F 0 "P3" H 7550 3250 50  0000 C CNN
F 1 "CONN_01X02" V 7650 3100 50  0000 C CNN
F 2 "Connect:bornier2" H 7550 3100 50  0001 C CNN
F 3 "" H 7550 3100 50  0000 C CNN
	1    7550 3100
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P4
U 1 1 5965D853
P 7550 3750
F 0 "P4" H 7550 3900 50  0000 C CNN
F 1 "CONN_01X02" V 7650 3750 50  0000 C CNN
F 2 "Connect:bornier2" H 7550 3750 50  0001 C CNN
F 3 "" H 7550 3750 50  0000 C CNN
	1    7550 3750
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P5
U 1 1 5965D874
P 7550 4400
F 0 "P5" H 7550 4550 50  0000 C CNN
F 1 "CONN_01X02" V 7650 4400 50  0000 C CNN
F 2 "Connect:bornier2" H 7550 4400 50  0001 C CNN
F 3 "" H 7550 4400 50  0000 C CNN
	1    7550 4400
	1    0    0    -1  
$EndComp
$Comp
L LD1117S33TR U1
U 1 1 5965DAC6
P 5500 2200
F 0 "U1" H 5500 2450 50  0000 C CNN
F 1 "LD1117S33TR" H 5500 2400 50  0000 C CNN
F 2 "TO_SOT_Packages_THT:TO-220_Neutral123_Vertical_LargePads" H 5500 2300 50  0000 C CNN
F 3 "" H 5500 2200 50  0000 C CNN
	1    5500 2200
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 5965DB0F
P 4650 2300
F 0 "C1" H 4675 2400 50  0000 L CNN
F 1 "100nF" H 4675 2200 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Rect_L4_W2.5_P2.5" H 4688 2150 50  0001 C CNN
F 3 "" H 4650 2300 50  0000 C CNN
	1    4650 2300
	1    0    0    -1  
$EndComp
$Comp
L CP C2
U 1 1 5965DB67
P 6300 2300
F 0 "C2" H 6325 2400 50  0000 L CNN
F 1 "10uF" H 6325 2200 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D5_L6_P2.5" H 6338 2150 50  0001 C CNN
F 3 "" H 6300 2300 50  0000 C CNN
	1    6300 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 2150 6300 2150
Wire Wire Line
	4650 2450 6300 2450
Wire Wire Line
	4650 2150 5100 2150
Connection ~ 5500 2450
Wire Wire Line
	7350 2400 6850 2400
Wire Wire Line
	7350 2500 6850 2500
Wire Wire Line
	7350 3050 6900 3050
Wire Wire Line
	7350 3150 6900 3150
Wire Wire Line
	7350 3700 6900 3700
Wire Wire Line
	7350 3800 6900 3800
Wire Wire Line
	7350 4350 6900 4350
Wire Wire Line
	7350 4450 6900 4450
Wire Wire Line
	6250 3350 5800 3350
Wire Wire Line
	6250 3450 5800 3450
$Comp
L GND #PWR01
U 1 1 5965E004
P 4200 3950
F 0 "#PWR01" H 4200 3700 50  0001 C CNN
F 1 "GND" H 4200 3800 50  0000 C CNN
F 2 "" H 4200 3950 50  0000 C CNN
F 3 "" H 4200 3950 50  0000 C CNN
	1    4200 3950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR02
U 1 1 5965E030
P 4700 3100
F 0 "#PWR02" H 4700 2950 50  0001 C CNN
F 1 "VCC" H 4700 3250 50  0000 C CNN
F 2 "" H 4700 3100 50  0000 C CNN
F 3 "" H 4700 3100 50  0000 C CNN
	1    4700 3100
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG03
U 1 1 5965E05C
P 4200 3100
F 0 "#FLG03" H 4200 3195 50  0001 C CNN
F 1 "PWR_FLAG" H 4200 3280 50  0000 C CNN
F 2 "" H 4200 3100 50  0000 C CNN
F 3 "" H 4200 3100 50  0000 C CNN
	1    4200 3100
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG04
U 1 1 5965E088
P 4700 3950
F 0 "#FLG04" H 4700 4045 50  0001 C CNN
F 1 "PWR_FLAG" H 4700 4130 50  0000 C CNN
F 2 "" H 4700 3950 50  0000 C CNN
F 3 "" H 4700 3950 50  0000 C CNN
	1    4700 3950
	-1   0    0    1   
$EndComp
Wire Wire Line
	4700 3100 4700 3950
Wire Wire Line
	4200 3950 4200 3100
Text Label 4700 3400 0    60   ~ 0
VI
Text Label 5900 3350 0    60   ~ 0
GND
Text Label 7000 2400 0    60   ~ 0
GND
Text Label 7050 3050 0    60   ~ 0
GND
Text Label 7050 3700 0    60   ~ 0
GND
Text Label 7050 4350 0    60   ~ 0
GND
Text Label 7050 4450 0    60   ~ 0
VI
Text Label 7100 3800 0    60   ~ 0
VI
Text Label 5950 3450 0    60   ~ 0
VI
Text Label 7050 3150 0    60   ~ 0
VO
Text Label 7000 2500 0    60   ~ 0
VO
Text Label 5950 2150 0    60   ~ 0
VO
Text Label 4850 2150 0    60   ~ 0
VI
Text Label 5000 2450 0    60   ~ 0
GND
Text Label 5800 2450 0    60   ~ 0
GND
$Comp
L CONN_01X01 P6
U 1 1 5965F298
P 2800 1900
F 0 "P6" H 2800 2000 50  0000 C CNN
F 1 "CONN_01X01" V 2900 1900 50  0000 C CNN
F 2 "drillhole:hole" H 2800 1900 50  0001 C CNN
F 3 "" H 2800 1900 50  0000 C CNN
	1    2800 1900
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P7
U 1 1 5965F2C8
P 2800 2450
F 0 "P7" H 2800 2550 50  0000 C CNN
F 1 "CONN_01X01" V 2900 2450 50  0000 C CNN
F 2 "drillhole:hole" H 2800 2450 50  0001 C CNN
F 3 "" H 2800 2450 50  0000 C CNN
	1    2800 2450
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P8
U 1 1 5965F305
P 2800 3050
F 0 "P8" H 2800 3150 50  0000 C CNN
F 1 "CONN_01X01" V 2900 3050 50  0000 C CNN
F 2 "drillhole:hole" H 2800 3050 50  0001 C CNN
F 3 "" H 2800 3050 50  0000 C CNN
	1    2800 3050
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P9
U 1 1 5965F33B
P 2800 3650
F 0 "P9" H 2800 3750 50  0000 C CNN
F 1 "CONN_01X01" V 2900 3650 50  0000 C CNN
F 2 "drillhole:hole" H 2800 3650 50  0001 C CNN
F 3 "" H 2800 3650 50  0000 C CNN
	1    2800 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 1900 2100 1900
Wire Wire Line
	2600 2450 2100 2450
Wire Wire Line
	2600 3050 2100 3050
Wire Wire Line
	2600 3650 2100 3650
Text Label 2300 1900 0    60   ~ 0
GND
Text Label 2300 2450 0    60   ~ 0
GND
Text Label 2300 3050 0    60   ~ 0
GND
Text Label 2300 3650 0    60   ~ 0
GND
$EndSCHEMATC