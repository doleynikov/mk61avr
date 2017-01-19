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
LIBS:special
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
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "MK-61-Avr - simulator of programmable calculator MK-61"
Date "18 jan 2017"
Rev "1.0"
Comp "Private hobby project"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 6500 7500 0    60   ~ 0
COL_9
Text Label 6000 7500 0    60   ~ 0
COL_8
Text Label 5550 7500 0    60   ~ 0
COL_7
Text Label 5050 7500 0    60   ~ 0
COL_6
Text Label 4600 7500 0    60   ~ 0
COL_5
Text Label 4100 7500 0    60   ~ 0
COL_4
Text Label 3600 7500 0    60   ~ 0
COL_3
Text Label 3100 7500 0    60   ~ 0
COL_2
Text Label 2650 7500 0    60   ~ 0
COL_1
Text Label 2150 7500 0    60   ~ 0
COL_0
Text Label 700  6900 0    60   ~ 0
ROW_2
Text Label 700  6500 0    60   ~ 0
ROW_1
Text Label 700  6100 0    60   ~ 0
ROW_0
Text Label 700  5700 0    60   ~ 0
ROW_3
Text Notes 3300 5500 0    60   ~ 0
MK52 keys
Entry Wire Line
	600  5800 700  5700
$Comp
L R R1
U 1 1 4B0C0732
P 1400 5700
F 0 "R1" V 1480 5700 50  0000 C CNN
F 1 "680" V 1400 5700 50  0000 C CNN
F 2 "" H 1400 5700 60  0001 C CNN
F 3 "" H 1400 5700 60  0001 C CNN
	1    1400 5700
	0    1    1    0   
$EndComp
$Comp
L SW_PUSH_SMALL SW17
U 1 1 4B0C072C
P 3850 5900
F 0 "SW17" H 4000 6010 30  0000 C CNN
F 1 "LOAD" H 3850 5821 30  0000 C CNN
F 2 "" H 3850 5900 60  0001 C CNN
F 3 "" H 3850 5900 60  0001 C CNN
F 4 "" H 3800 5900 30  0000 C CNB "Êîä"
	1    3850 5900
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW13
U 1 1 4B0C072B
P 3350 5900
F 0 "SW13" H 3500 6010 30  0000 C CNN
F 1 "SAVE" H 3350 5821 30  0000 C CNN
F 2 "" H 3350 5900 60  0001 C CNN
F 3 "" H 3350 5900 60  0001 C CNN
F 4 "" H 3300 5900 30  0000 C CNB "Êîä"
	1    3350 5900
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW9
U 1 1 4B0C072A
P 2850 5900
F 0 "SW9" H 3000 6010 30  0000 C CNN
F 1 "RGT" H 2850 5821 30  0000 C CNN
F 2 "" H 2850 5900 60  0001 C CNN
F 3 "" H 2850 5900 60  0001 C CNN
F 4 "" H 2800 5900 30  0000 C CNB "Êîä"
	1    2850 5900
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW5
U 1 1 4B0C0729
P 2400 5900
F 0 "SW5" H 2550 6010 30  0000 C CNN
F 1 "LFT" H 2400 5821 30  0000 C CNN
F 2 "" H 2400 5900 60  0001 C CNN
F 3 "" H 2400 5900 60  0001 C CNN
F 4 "" H 2350 5900 30  0000 C CNB "Êîä"
	1    2400 5900
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW1
U 1 1 4B0C0728
P 1900 5900
F 0 "SW1" H 2050 6010 30  0000 C CNN
F 1 "ON/OFF" H 1900 5821 30  0000 C CNN
F 2 "" H 1900 5900 60  0001 C CNN
F 3 "" H 1900 5900 60  0001 C CNN
F 4 "" H 1850 5900 30  0000 C CNB "Êîä"
	1    1900 5900
	1    0    0    -1  
$EndComp
Entry Wire Line
	6400 7600 6500 7500
Entry Wire Line
	5900 7600 6000 7500
Entry Wire Line
	5450 7600 5550 7500
Entry Wire Line
	4950 7600 5050 7500
Entry Wire Line
	4500 7600 4600 7500
Entry Wire Line
	4000 7600 4100 7500
Entry Wire Line
	3500 7600 3600 7500
Entry Wire Line
	3000 7600 3100 7500
Entry Wire Line
	2550 7600 2650 7500
Entry Wire Line
	2050 7600 2150 7500
Entry Wire Line
	600  7000 700  6900
Entry Wire Line
	600  6600 700  6500
$Comp
L R R4
U 1 1 4AE804E0
P 1400 6900
F 0 "R4" V 1480 6900 50  0000 C CNN
F 1 "680" V 1400 6900 50  0000 C CNN
F 2 "" H 1400 6900 60  0001 C CNN
F 3 "" H 1400 6900 60  0001 C CNN
	1    1400 6900
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 4AE804AA
P 1400 6500
F 0 "R3" V 1480 6500 50  0000 C CNN
F 1 "680" V 1400 6500 50  0000 C CNN
F 2 "" H 1400 6500 60  0001 C CNN
F 3 "" H 1400 6500 60  0001 C CNN
	1    1400 6500
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 4AE80428
P 1400 6100
F 0 "R2" V 1480 6100 50  0000 C CNN
F 1 "680" V 1400 6100 50  0000 C CNN
F 2 "" H 1400 6100 60  0001 C CNN
F 3 "" H 1400 6100 60  0001 C CNN
	1    1400 6100
	0    1    1    0   
$EndComp
Entry Wire Line
	600  6200 700  6100
$Comp
L SW_PUSH_SMALL SW35
U 1 1 4AE80219
P 6250 7100
F 0 "SW35" H 6400 7210 30  0000 C CNN
F 1 "F" H 6250 7021 30  0000 C CNN
F 2 "" H 6250 7100 60  0001 C CNN
F 3 "" H 6250 7100 60  0001 C CNN
F 4 "21H" H 6200 7100 30  0000 C CNB "Êîä"
	1    6250 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW34
U 1 1 4AE80218
P 6250 6700
F 0 "SW34" H 6400 6810 30  0000 C CNN
F 1 "Bx" H 6250 6621 30  0000 C CNN
F 2 "" H 6250 6700 60  0001 C CNN
F 3 "" H 6250 6700 60  0001 C CNN
F 4 "81H" H 6200 6700 30  0000 C CNB "Êîä"
	1    6250 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW33
U 1 1 4AE80217
P 6250 6300
F 0 "SW33" H 6400 6410 30  0000 C CNN
F 1 "9" H 6250 6221 30  0000 C CNN
F 2 "" H 6250 6300 60  0001 C CNN
F 3 "" H 6250 6300 60  0001 C CNN
F 4 "41H" H 6200 6300 30  0000 C CNB "Êîä"
	1    6250 6300
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW32
U 1 1 4AE80216
P 5750 7100
F 0 "SW32" H 5900 7210 30  0000 C CNN
F 1 "K" H 5750 7021 30  0000 C CNN
F 2 "" H 5750 7100 60  0001 C CNN
F 3 "" H 5750 7100 60  0001 C CNN
F 4 "29H" H 5700 7100 30  0000 C CNB "Êîä"
	1    5750 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW31
U 1 1 4AE80215
P 5750 6700
F 0 "SW31" H 5900 6810 30  0000 C CNN
F 1 "Cx" H 5750 6621 30  0000 C CNN
F 2 "" H 5750 6700 60  0001 C CNN
F 3 "" H 5750 6700 60  0001 C CNN
F 4 "89H" H 5700 6700 30  0000 C CNB "Êîä"
	1    5750 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW30
U 1 1 4AE80214
P 5750 6300
F 0 "SW30" H 5900 6410 30  0000 C CNN
F 1 "8" H 5750 6221 30  0000 C CNN
F 2 "" H 5750 6300 60  0001 C CNN
F 3 "" H 5750 6300 60  0001 C CNN
F 4 "49H" H 5700 6300 30  0000 C CNB "Êîä"
	1    5750 6300
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW29
U 1 1 4AE8011A
P 5300 7100
F 0 "SW29" H 5450 7210 30  0000 C CNN
F 1 "<STEP" H 5300 7021 30  0000 C CNN
F 2 "" H 5300 7100 60  0001 C CNN
F 3 "" H 5300 7100 60  0001 C CNN
F 4 "28H" H 5250 7100 30  0000 C CNB "Êîä"
	1    5300 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW28
U 1 1 4AE80119
P 5300 6700
F 0 "SW28" H 5450 6810 30  0000 C CNN
F 1 "POW" H 5300 6621 30  0000 C CNN
F 2 "" H 5300 6700 60  0001 C CNN
F 3 "" H 5300 6700 60  0001 C CNN
F 4 "88H" H 5250 6700 30  0000 C CNB "Êîä"
	1    5300 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW27
U 1 1 4AE80118
P 5300 6300
F 0 "SW27" H 5450 6410 30  0000 C CNN
F 1 "7" H 5300 6221 30  0000 C CNN
F 2 "" H 5300 6300 60  0001 C CNN
F 3 "" H 5300 6300 60  0001 C CNN
F 4 "48H" H 5250 6300 30  0000 C CNB "Êîä"
	1    5300 6300
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW26
U 1 1 4AE80117
P 4800 7100
F 0 "SW26" H 4950 7210 30  0000 C CNN
F 1 "LOAD" H 4800 7021 30  0000 C CNN
F 2 "" H 4800 7100 60  0001 C CNN
F 3 "" H 4800 7100 60  0001 C CNN
F 4 "27H" H 4750 7100 30  0000 C CNB "Êîä"
	1    4800 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW25
U 1 1 4AE80116
P 4800 6700
F 0 "SW25" H 4950 6810 30  0000 C CNN
F 1 "/-/" H 4800 6621 30  0000 C CNN
F 2 "" H 4800 6700 60  0001 C CNN
F 3 "" H 4800 6700 60  0001 C CNN
F 4 "87H" H 4750 6700 30  0000 C CNB "Êîä"
	1    4800 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW24
U 1 1 4AE80115
P 4800 6300
F 0 "SW24" H 4950 6410 30  0000 C CNN
F 1 "6" H 4800 6221 30  0000 C CNN
F 2 "" H 4800 6300 60  0001 C CNN
F 3 "" H 4800 6300 60  0001 C CNN
F 4 "47H" H 4750 6300 30  0000 C CNB "Êîä"
	1    4800 6300
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW23
U 1 1 4AE80114
P 4350 7100
F 0 "SW23" H 4500 7210 30  0000 C CNN
F 1 "STEP>" H 4350 7021 30  0000 C CNN
F 2 "" H 4350 7100 60  0001 C CNN
F 3 "" H 4350 7100 60  0001 C CNN
F 4 "26H" H 4300 7100 30  0000 C CNB "Êîä"
	1    4350 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW22
U 1 1 4AE80113
P 4350 6700
F 0 "SW22" H 4500 6810 30  0000 C CNN
F 1 "," H 4350 6621 30  0000 C CNN
F 2 "" H 4350 6700 60  0001 C CNN
F 3 "" H 4350 6700 60  0001 C CNN
F 4 "86H" H 4300 6700 30  0000 C CNB "Êîä"
	1    4350 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW21
U 1 1 4AE80112
P 4350 6300
F 0 "SW21" H 4500 6410 30  0000 C CNN
F 1 "5" H 4350 6221 30  0000 C CNN
F 2 "" H 4350 6300 60  0001 C CNN
F 3 "" H 4350 6300 60  0001 C CNN
F 4 "46H" H 4300 6300 30  0000 C CNB "Êîä"
	1    4350 6300
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW20
U 1 1 4AE80111
P 3850 7100
F 0 "SW20" H 4000 7210 30  0000 C CNN
F 1 "STORE" H 3850 7021 30  0000 C CNN
F 2 "" H 3850 7100 60  0001 C CNN
F 3 "" H 3850 7100 60  0001 C CNN
F 4 "25H" H 3800 7100 30  0000 C CNB "Êîä"
	1    3850 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW19
U 1 1 4AE80110
P 3850 6700
F 0 "SW19" H 4000 6810 30  0000 C CNN
F 1 "XY" H 3850 6621 30  0000 C CNN
F 2 "" H 3850 6700 60  0001 C CNN
F 3 "" H 3850 6700 60  0001 C CNN
F 4 "85H" H 3800 6700 30  0000 C CNB "Êîä"
	1    3850 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW18
U 1 1 4AE8010F
P 3850 6300
F 0 "SW18" H 4000 6410 30  0000 C CNN
F 1 "4" H 3850 6221 30  0000 C CNN
F 2 "" H 3850 6300 60  0001 C CNN
F 3 "" H 3850 6300 60  0001 C CNN
F 4 "45H" H 3800 6300 30  0000 C CNB "Êîä"
	1    3850 6300
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW16
U 1 1 4AE800A1
P 3350 7100
F 0 "SW16" H 3500 7210 30  0000 C CNN
F 1 "SUB" H 3350 7021 30  0000 C CNN
F 2 "" H 3350 7100 60  0001 C CNN
F 3 "" H 3350 7100 60  0001 C CNN
F 4 "2AH" H 3300 7100 30  0000 C CNB "Êîä"
	1    3350 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW15
U 1 1 4AE800A0
P 3350 6700
F 0 "SW15" H 3500 6810 30  0000 C CNN
F 1 "3" H 3350 6621 30  0000 C CNN
F 2 "" H 3350 6700 60  0001 C CNN
F 3 "" H 3350 6700 60  0001 C CNN
F 4 "8A" H 3300 6700 30  0000 C CNB "Êîä"
	1    3350 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW14
U 1 1 4AE8009F
P 3350 6300
F 0 "SW14" H 3500 6410 30  0000 C CNN
F 1 "3" H 3350 6221 30  0000 C CNN
F 2 "" H 3350 6300 60  0001 C CNN
F 3 "" H 3350 6300 60  0001 C CNN
F 4 "4AH" H 3300 6300 30  0000 C CNB "Êîä"
	1    3350 6300
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW12
U 1 1 4AE8009E
P 2850 7100
F 0 "SW12" H 3000 7210 30  0000 C CNN
F 1 "RET" H 2850 7021 30  0000 C CNN
F 2 "" H 2850 7100 60  0001 C CNN
F 3 "" H 2850 7100 60  0001 C CNN
F 4 "24H" H 2800 7100 30  0000 C CNB "Êîä"
	1    2850 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW11
U 1 1 4AE8009D
P 2850 6700
F 0 "SW11" H 3000 6810 30  0000 C CNN
F 1 "X" H 2850 6621 30  0000 C CNN
F 2 "" H 2850 6700 60  0001 C CNN
F 3 "" H 2850 6700 60  0001 C CNN
F 4 "84H" H 2800 6700 30  0000 C CNB "Êîä"
	1    2850 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW10
U 1 1 4AE8009C
P 2850 6300
F 0 "SW10" H 3000 6410 30  0000 C CNN
F 1 "2" H 2850 6221 30  0000 C CNN
F 2 "" H 2850 6300 60  0001 C CNN
F 3 "" H 2850 6300 60  0001 C CNN
F 4 "44H" H 2800 6300 30  0000 C CNB "Êîä"
	1    2850 6300
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW8
U 1 1 4AE80044
P 2400 7100
F 0 "SW8" H 2550 7210 30  0000 C CNN
F 1 "GOTO" H 2400 7021 30  0000 C CNN
F 2 "" H 2400 7100 60  0001 C CNN
F 3 "" H 2400 7100 60  0001 C CNN
F 4 "23H" H 2350 7100 30  0000 C CNB "Êîä"
	1    2400 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW7
U 1 1 4AE80043
P 2400 6700
F 0 "SW7" H 2550 6810 30  0000 C CNN
F 1 "-" H 2400 6621 30  0000 C CNN
F 2 "" H 2400 6700 60  0001 C CNN
F 3 "" H 2400 6700 60  0001 C CNN
F 4 "83H" H 2350 6700 30  0000 C CNB "Êîä"
	1    2400 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW6
U 1 1 4AE80042
P 2400 6300
F 0 "SW6" H 2550 6410 30  0000 C CNN
F 1 "1" H 2400 6221 30  0000 C CNN
F 2 "" H 2400 6300 60  0001 C CNN
F 3 "" H 2400 6300 60  0001 C CNN
F 4 "43H" H 2350 6300 30  0000 C CNB "Êîä"
	1    2400 6300
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW4
U 1 1 4AE7FFB3
P 1900 7100
F 0 "SW4" H 2050 7210 30  0000 C CNN
F 1 "S/S" H 1900 7021 30  0000 C CNN
F 2 "" H 1900 7100 60  0001 C CNN
F 3 "" H 1900 7100 60  0001 C CNN
F 4 "22H" H 1850 7100 30  0000 C CNB "Êîä"
	1    1900 7100
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW3
U 1 1 4AE7FFAD
P 1900 6700
F 0 "SW3" H 2050 6810 30  0000 C CNN
F 1 "+" H 1900 6621 30  0000 C CNN
F 2 "" H 1900 6700 60  0001 C CNN
F 3 "" H 1900 6700 60  0001 C CNN
F 4 "82H" H 1850 6700 30  0000 C CNB "Êîä"
	1    1900 6700
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH_SMALL SW2
U 1 1 4AE7FFA4
P 1900 6300
F 0 "SW2" H 2050 6410 30  0000 C CNN
F 1 "0" H 1900 6221 30  0000 C CNN
F 2 "" H 1900 6300 60  0001 C CNN
F 3 "" H 1900 6300 60  0001 C CNN
F 4 "42H" H 1850 6300 30  0000 C CNB "Êîä"
	1    1900 6300
	1    0    0    -1  
$EndComp
$Comp
L ATMEGA8-P IC1
U 1 1 587DB7C3
P 4370 3650
F 0 "IC1" H 3620 4950 40  0000 L BNN
F 1 "ATMEGA8-P" H 4870 2200 40  0000 L BNN
F 2 "DIL28" H 4370 3650 30  0000 C CIN
F 3 "" H 4370 3650 60  0000 C CNN
	1    4370 3650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 587DB8ED
P 4370 5300
F 0 "#PWR01" H 4370 5300 30  0001 C CNN
F 1 "GND" H 4370 5230 30  0001 C CNN
F 2 "" H 4370 5300 60  0000 C CNN
F 3 "" H 4370 5300 60  0000 C CNN
	1    4370 5300
	1    0    0    -1  
$EndComp
Entry Wire Line
	6800 3250 6900 3350
Entry Wire Line
	6800 3350 6900 3450
Entry Wire Line
	6800 2550 6900 2650
Entry Wire Line
	6800 2650 6900 2750
Entry Wire Line
	6800 2750 6900 2850
Entry Wire Line
	6800 2850 6900 2950
Entry Wire Line
	6800 2950 6900 3050
Entry Wire Line
	6800 3050 6900 3150
Entry Wire Line
	6800 1600 6900 1700
Entry Wire Line
	6800 1700 6900 1800
Entry Wire Line
	6800 4150 6900 4250
Entry Wire Line
	6800 4250 6900 4350
Entry Wire Line
	6800 4350 6900 4450
Entry Wire Line
	6800 4450 6900 4550
Text Label 6552 2550 0    60   ~ 0
COL_0
Text Label 6550 2650 0    60   ~ 0
COL_1
Text Label 6550 2750 0    60   ~ 0
COL_2
Text Label 6550 2850 0    60   ~ 0
COL_3
Text Label 6554 2950 0    60   ~ 0
COL_4
Text Label 6550 3050 0    60   ~ 0
COL_5
Text Label 6250 1700 0    60   ~ 0
COL_6
Text Label 6550 1600 0    60   ~ 0
COL_7
Text Label 6550 3250 0    60   ~ 0
COL_8
Text Label 6550 3350 0    60   ~ 0
COL_9
$Comp
L PWR_FLAG #FLG02
U 1 1 587DEA34
P 1552 3072
F 0 "#FLG02" H 1552 3167 30  0001 C CNN
F 1 "PWR_FLAG" H 1552 3252 30  0000 C CNN
F 2 "" H 1552 3072 60  0000 C CNN
F 3 "" H 1552 3072 60  0000 C CNN
	1    1552 3072
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG03
U 1 1 587DEA43
P 1292 3068
F 0 "#FLG03" H 1292 3163 30  0001 C CNN
F 1 "PWR_FLAG" H 1292 3248 30  0000 C CNN
F 2 "" H 1292 3068 60  0000 C CNN
F 3 "" H 1292 3068 60  0000 C CNN
	1    1292 3068
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR04
U 1 1 587DEA53
P 1292 3064
F 0 "#PWR04" H 1292 3164 30  0001 C CNN
F 1 "VCC" H 1292 3164 30  0000 C CNN
F 2 "" H 1292 3064 60  0000 C CNN
F 3 "" H 1292 3064 60  0000 C CNN
	1    1292 3064
	1    0    0    1   
$EndComp
$Comp
L GND #PWR05
U 1 1 587DEABF
P 1552 3072
F 0 "#PWR05" H 1552 3072 30  0001 C CNN
F 1 "GND" H 1552 3002 30  0001 C CNN
F 2 "" H 1552 3072 60  0000 C CNN
F 3 "" H 1552 3072 60  0000 C CNN
	1    1552 3072
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR06
U 1 1 587DEAF4
P 3970 2212
F 0 "#PWR06" H 3970 2312 30  0001 C CNN
F 1 "VCC" H 3970 2312 30  0000 C CNN
F 2 "" H 3970 2212 60  0000 C CNN
F 3 "" H 3970 2212 60  0000 C CNN
	1    3970 2212
	1    0    0    -1  
$EndComp
Text Label 6550 4150 0    60   ~ 0
ROW_0
Text Label 6550 4250 0    60   ~ 0
ROW_1
Text Label 6550 4350 0    60   ~ 0
ROW_2
Text Label 6550 4450 0    60   ~ 0
ROW_3
Text Label 6550 3950 0    60   ~ 0
RX
Text Label 6550 4050 0    60   ~ 0
TX
Text Label 6430 3750 0    60   ~ 0
I2C_SCL
Text Label 6430 3650 0    60   ~ 0
I2C_SDA
$Comp
L CONN_4 P1
U 1 1 587DF67B
P 7382 3482
F 0 "P1" V 7332 3482 50  0000 C CNN
F 1 "CONN_4" V 7432 3482 50  0000 C CNN
F 2 "" H 7382 3482 60  0000 C CNN
F 3 "" H 7382 3482 60  0000 C CNN
	1    7382 3482
	1    0    0    -1  
$EndComp
Text Label 7032 3332 0    60   ~ 0
VCC
Text Label 7032 3432 0    60   ~ 0
I2C_SDA
Text Label 7032 3532 0    60   ~ 0
I2C_SCL
Text Label 7032 3632 0    60   ~ 0
GND
Text Label 4370 5150 0    60   ~ 0
GND
Text Label 1292 3068 0    60   ~ 0
VCC
Text Label 1552 3072 0    60   ~ 0
GND
$Comp
L CONN_3 K1
U 1 1 587DF75B
P 7384 3906
F 0 "K1" V 7334 3906 50  0000 C CNN
F 1 "CONN_3" V 7434 3906 40  0000 C CNN
F 2 "" H 7384 3906 60  0000 C CNN
F 3 "" H 7384 3906 60  0000 C CNN
	1    7384 3906
	1    0    0    -1  
$EndComp
Text Label 7034 3806 0    60   ~ 0
GND
Text Label 7034 3906 0    60   ~ 0
RX
Text Label 7034 4006 0    60   ~ 0
TX
$Comp
L SW_PUSH SW36
U 1 1 587DF8F7
P 4820 1900
F 0 "SW36" H 4970 2010 50  0000 C CNN
F 1 "SW_PUSH" H 4820 1820 50  0000 C CNN
F 2 "~" H 4820 1900 60  0000 C CNN
F 3 "~" H 4820 1900 60  0000 C CNN
	1    4820 1900
	1    0    0    -1  
$EndComp
$Comp
L CONN_5X2 P2
U 1 1 587F32F4
P 8450 2400
F 0 "P2" H 8450 2700 60  0000 C CNN
F 1 "CONN_5X2" V 8450 2400 50  0000 C CNN
F 2 "" H 8450 2400 60  0000 C CNN
F 3 "" H 8450 2400 60  0000 C CNN
	1    8450 2400
	1    0    0    -1  
$EndComp
Text Label 8850 2300 0    60   ~ 0
GND
Text Label 8850 2400 0    60   ~ 0
GND
Text Label 8850 2500 0    60   ~ 0
GND
Text Label 8850 2600 0    60   ~ 0
GND
Text Label 8050 2200 0    60   ~ 0
MOSI
Text Label 8050 2400 0    60   ~ 0
RST
Text Label 8050 2500 0    60   ~ 0
SCK
Text Label 8050 2600 0    60   ~ 0
MISO
Text Label 3700 1900 0    60   ~ 0
RST
$Comp
L GND #PWR?
U 1 1 5880F0CB
P 3370 3000
F 0 "#PWR?" H 3370 3000 30  0001 C CNN
F 1 "GND" H 3370 2930 30  0001 C CNN
F 2 "" H 3370 3000 60  0000 C CNN
F 3 "" H 3370 3000 60  0000 C CNN
	1    3370 3000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 588119AE
P 5240 1980
F 0 "#PWR?" H 5240 1980 30  0001 C CNN
F 1 "GND" H 5240 1910 30  0001 C CNN
F 2 "" H 5240 1980 60  0000 C CNN
F 3 "" H 5240 1980 60  0000 C CNN
	1    5240 1980
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 58811AF2
P 4370 2080
F 0 "R1" V 4450 2080 50  0000 C CNN
F 1 "10K" V 4370 2080 50  0000 C CNN
F 2 "" V 4300 2080 50  0000 C CNN
F 3 "" H 4370 2080 50  0000 C CNN
	1    4370 2080
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 6500 700  6500
Wire Wire Line
	1150 5700 700  5700
Wire Notes Line
	4150 5550 3150 5550
Wire Notes Line
	4150 5550 4150 6050
Wire Notes Line
	4150 6050 3150 6050
Wire Notes Line
	3150 6050 3150 5550
Wire Wire Line
	3750 5700 3750 5800
Connection ~ 4100 6400
Wire Wire Line
	4100 6000 4100 7500
Wire Wire Line
	4100 6000 3950 6000
Connection ~ 3100 6400
Wire Wire Line
	3100 6000 3100 7500
Wire Wire Line
	3100 6000 2950 6000
Connection ~ 2150 6400
Wire Wire Line
	2150 6000 2150 7500
Wire Wire Line
	2150 6000 2000 6000
Wire Wire Line
	1650 6100 6150 6100
Connection ~ 6500 6800
Wire Wire Line
	6350 6800 6500 6800
Connection ~ 6000 7200
Wire Wire Line
	6000 7200 5850 7200
Wire Wire Line
	6000 6400 6000 7500
Wire Wire Line
	6000 6400 5850 6400
Connection ~ 5550 6800
Wire Wire Line
	5400 6800 5550 6800
Connection ~ 5050 7200
Wire Wire Line
	5050 7200 4900 7200
Wire Wire Line
	5050 6400 5050 7500
Wire Wire Line
	5050 6400 4900 6400
Connection ~ 4600 6800
Wire Wire Line
	4450 6800 4600 6800
Connection ~ 4100 6800
Wire Wire Line
	4100 6800 3950 6800
Wire Wire Line
	3950 6400 4100 6400
Connection ~ 3600 7200
Wire Wire Line
	3600 7200 3450 7200
Connection ~ 3100 6800
Wire Wire Line
	3100 6800 2950 6800
Wire Wire Line
	2950 6400 3100 6400
Connection ~ 2650 6800
Wire Wire Line
	2650 6800 2500 6800
Wire Wire Line
	2650 6400 2500 6400
Connection ~ 2150 7200
Wire Wire Line
	2150 7200 2000 7200
Wire Wire Line
	2000 6400 2150 6400
Connection ~ 5200 6900
Wire Wire Line
	5200 6900 5200 7000
Connection ~ 4250 6900
Wire Wire Line
	4250 6900 4250 7000
Connection ~ 3250 6900
Wire Wire Line
	3250 6900 3250 7000
Connection ~ 2300 6900
Wire Wire Line
	2300 6900 2300 7000
Wire Wire Line
	6150 6900 6150 7000
Connection ~ 5200 6500
Wire Wire Line
	5200 6500 5200 6600
Connection ~ 4250 6500
Wire Wire Line
	4250 6500 4250 6600
Connection ~ 3250 6500
Wire Wire Line
	3250 6500 3250 6600
Connection ~ 2300 6500
Wire Wire Line
	2300 6500 2300 6600
Wire Wire Line
	6150 6500 6150 6600
Connection ~ 4700 6100
Wire Wire Line
	4700 6100 4700 6200
Connection ~ 3750 6100
Wire Wire Line
	3750 6100 3750 6200
Connection ~ 2750 6100
Wire Wire Line
	2750 6100 2750 6200
Connection ~ 1800 6100
Wire Wire Line
	1800 6200 1800 6100
Wire Wire Line
	5650 6100 5650 6200
Wire Wire Line
	2300 6100 2300 6200
Connection ~ 2300 6100
Wire Wire Line
	3250 6100 3250 6200
Connection ~ 3250 6100
Wire Wire Line
	4250 6100 4250 6200
Connection ~ 4250 6100
Wire Wire Line
	5200 6100 5200 6200
Connection ~ 5200 6100
Wire Wire Line
	6150 6100 6150 6200
Connection ~ 5650 6100
Wire Wire Line
	1800 6500 1800 6600
Connection ~ 1800 6500
Wire Wire Line
	2750 6500 2750 6600
Connection ~ 2750 6500
Wire Wire Line
	3750 6500 3750 6600
Connection ~ 3750 6500
Wire Wire Line
	4700 6500 4700 6600
Connection ~ 4700 6500
Wire Wire Line
	5650 6500 5650 6600
Connection ~ 5650 6500
Wire Wire Line
	1800 6900 1800 7000
Connection ~ 1800 6900
Wire Wire Line
	2750 6900 2750 7000
Connection ~ 2750 6900
Wire Wire Line
	3750 6900 3750 7000
Connection ~ 3750 6900
Wire Wire Line
	4700 6900 4700 7000
Connection ~ 4700 6900
Wire Wire Line
	5650 6900 5650 7000
Connection ~ 5650 6900
Wire Wire Line
	1650 6900 6150 6900
Wire Wire Line
	2150 6800 2000 6800
Connection ~ 2150 6800
Wire Wire Line
	2650 7200 2500 7200
Connection ~ 2650 7200
Wire Wire Line
	3100 7200 2950 7200
Connection ~ 3100 7200
Wire Wire Line
	3600 6400 3450 6400
Wire Wire Line
	3600 6800 3450 6800
Connection ~ 3600 6800
Wire Wire Line
	4100 7200 3950 7200
Connection ~ 4100 7200
Wire Wire Line
	4450 6400 4600 6400
Wire Wire Line
	4600 6400 4600 7500
Wire Wire Line
	4450 7200 4600 7200
Connection ~ 4600 7200
Wire Wire Line
	4900 6800 5050 6800
Connection ~ 5050 6800
Wire Wire Line
	5400 6400 5550 6400
Wire Wire Line
	5550 6400 5550 7500
Wire Wire Line
	5400 7200 5550 7200
Connection ~ 5550 7200
Wire Wire Line
	5850 6800 6000 6800
Connection ~ 6000 6800
Wire Wire Line
	6350 6400 6500 6400
Wire Wire Line
	6500 6400 6500 7500
Wire Wire Line
	6350 7200 6500 7200
Connection ~ 6500 7200
Wire Wire Line
	1650 6500 6150 6500
Wire Wire Line
	1650 5700 3750 5700
Connection ~ 2750 5700
Wire Wire Line
	2750 5700 2750 5800
Connection ~ 1800 5700
Wire Wire Line
	1800 5800 1800 5700
Wire Wire Line
	2300 5700 2300 5800
Connection ~ 2300 5700
Wire Wire Line
	3250 5700 3250 5800
Connection ~ 3250 5700
Wire Wire Line
	2500 6000 2650 6000
Wire Wire Line
	2650 6000 2650 7500
Connection ~ 2650 6400
Wire Wire Line
	3450 6000 3600 6000
Wire Wire Line
	3600 6000 3600 7500
Connection ~ 3600 6400
Wire Bus Line
	600  5750 600  7600
Wire Wire Line
	1150 6100 700  6100
Wire Wire Line
	1150 6900 700  6900
Wire Bus Line
	600  7600 6900 7600
Wire Bus Line
	6900 7600 6900 1700
Wire Wire Line
	5370 2550 6800 2550
Wire Wire Line
	3220 1700 6800 1700
Wire Wire Line
	3220 1700 3220 3250
Wire Wire Line
	3220 3250 3470 3250
Wire Wire Line
	4370 5300 4370 5150
Wire Wire Line
	3470 2950 3370 2950
Wire Wire Line
	3370 2950 3370 3000
Wire Wire Line
	3270 1900 4520 1900
Wire Wire Line
	3270 1900 3270 2550
Wire Wire Line
	3270 2550 3470 2550
Wire Wire Line
	4370 2250 3970 2250
Wire Wire Line
	3970 2250 3970 2210
Wire Wire Line
	5120 1900 5240 1900
Wire Wire Line
	5240 1900 5240 1980
Wire Wire Line
	4370 2230 4370 2250
Wire Wire Line
	4370 1930 4370 1900
Connection ~ 4370 1900
Wire Wire Line
	6800 1600 3170 1600
Wire Wire Line
	3170 1600 3170 3450
Wire Wire Line
	3170 3450 3470 3450
Wire Wire Line
	5370 2650 6800 2650
Wire Wire Line
	6800 3050 5370 3050
Wire Wire Line
	5370 2950 6800 2950
Wire Wire Line
	6800 2850 5370 2850
Wire Wire Line
	5370 2750 6800 2750
Wire Wire Line
	6800 3250 5370 3250
Wire Wire Line
	6800 3350 5370 3350
Wire Wire Line
	6800 4150 5370 4150
Wire Wire Line
	5370 4250 6800 4250
Wire Wire Line
	6800 4350 5370 4350
Wire Wire Line
	6800 4450 5370 4450
Wire Wire Line
	5370 3750 6800 3750
Wire Wire Line
	6800 3650 5370 3650
Entry Wire Line
	6800 3650 6900 3750
Entry Wire Line
	6800 3750 6900 3850
Wire Wire Line
	5370 4050 6800 4050
Wire Wire Line
	6800 3950 5370 3950
Entry Wire Line
	6800 3950 6900 4050
Entry Wire Line
	6800 4050 6900 4150
$EndSCHEMATC
