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
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L +5V #PWR?
U 1 1 58E0BF30
P 4850 2050
F 0 "#PWR?" H 4850 1900 50  0001 C CNN
F 1 "+5V" H 4850 2190 50  0000 C CNN
F 2 "" H 4850 2050 50  0000 C CNN
F 3 "" H 4850 2050 50  0000 C CNN
	1    4850 2050
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 58E0BF44
P 5750 2050
F 0 "#PWR?" H 5750 1900 50  0001 C CNN
F 1 "+5V" H 5750 2190 50  0000 C CNN
F 2 "" H 5750 2050 50  0000 C CNN
F 3 "" H 5750 2050 50  0000 C CNN
	1    5750 2050
	1    0    0    -1  
$EndComp
$Comp
L LED D?
U 1 1 58E0BF58
P 4850 2350
F 0 "D?" H 4850 2450 50  0000 C CNN
F 1 "LED" H 4850 2250 50  0000 C CNN
F 2 "" H 4850 2350 50  0000 C CNN
F 3 "" H 4850 2350 50  0000 C CNN
	1    4850 2350
	0    -1   -1   0   
$EndComp
$Comp
L R R?
U 1 1 58E0BFFD
P 4850 2800
F 0 "R?" V 4930 2800 50  0000 C CNN
F 1 "R" V 4850 2800 50  0000 C CNN
F 2 "" V 4780 2800 50  0000 C CNN
F 3 "" H 4850 2800 50  0000 C CNN
	1    4850 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 58E0C032
P 4850 3150
F 0 "#PWR?" H 4850 2900 50  0001 C CNN
F 1 "GND" H 4850 3000 50  0000 C CNN
F 2 "" H 4850 3150 50  0000 C CNN
F 3 "" H 4850 3150 50  0000 C CNN
	1    4850 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 2050 4850 2200
Wire Wire Line
	4850 2500 4850 2650
Wire Wire Line
	4850 2950 4850 3150
$Comp
L D_Photo D?
U 1 1 58E0C05C
P 5750 2400
F 0 "D?" H 5770 2470 50  0000 L CNN
F 1 "D_Photo" H 5710 2290 50  0000 C CNN
F 2 "" H 5700 2400 50  0000 C CNN
F 3 "" H 5700 2400 50  0000 C CNN
	1    5750 2400
	0    1    1    0   
$EndComp
Wire Wire Line
	5750 2050 5750 2200
Wire Wire Line
	5750 2500 5750 2750
Wire Wire Line
	5750 2750 6550 2750
Text Label 6550 2750 2    60   ~ 0
A0
$EndSCHEMATC
