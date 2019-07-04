# LedProc
Serial Processor for LED Patterns

## Concept
Controlling a bunch of LED Strips can be a challenge, linking them to a small microcontroller makes it simple to setup patterns, but to alter the patterns means recompiling and uploading new firmware. This project aims to allow patterns to be defined and executed via the USB Serial connection

The Teensy + OctoWS2811 breakout together work really well to drive the LED pixels, the intent is to hook one of those to a Raspberry Pi, or ESP8266 to provide wifi control, potentially with a simple web interface to allow defining new patterns, toggling between patterns, and altering parameters while patterns are running. 

## Overview
The teensy sketch will collect commands from serial, and run them immediately. And it will allow storing of commands to banks, which can be executed as required. Immediate commands will run when received, even if a bank is executing, allowing modification of parameters for the running bank.

## Protocol
*Note* Banks & VLed stuff not finished.. TODO ;)
```
(VLED mapping, n must be const.)
Vn - enable/disable vled mappings
 eg.
  V0 //disable vled mappings
  V1 //enable
(n,x must be constants, less than led max)
Pn:x - map from vled # to real led #
 eg.
  P1:P2:4P3:6 //map 1->2 2->4 3->6

(LED commands n/rgb can be const or !indirect as per regs)
Ln:R,G,B - make led N be RGB
  L123:100,200,255 //set LED 123 to R(100),G(200),B(255)
Wn - wait for N microseconds
U1 - request update of leds

(Register commands, n & x can be reg or indirect)
Rn:x - set register N to value X (N 0:255, X:int)
 eg.
  R1:200
  R1:!5 (set reg 1 to value of reg 5)
An:x - add X to reg N (result in reg N)
 eg.
  A1:1 (add 1 to reg 1)
  A1:!2 (add reg2 value to reg1)
Sn:x - sub X from reg N (result in reg N)
Mn:x - multiply reg N by X (result in reg N) 
Dn:x - divide reg N by X (result in reg N)

(2 reg comparisons, x can be indirect)
Ex - evaluate reg X
Jn - mark jump point N (N range of 0-1024)
JGx:n - jump to point N if reg X greater than last eval reg value
JLx:n - if less than
JEx:n - if equal
JNx:n - if not equal

(single reg jumps)
JZx:n - jump to point N if reg X is zero
JAn - always jump to point N

(bank commands, n can be indirect)
BRn - record next commands to bank N (N range 0-255) (do not nest!)
BS - stop bank recording
JBn - jump to bank N
```

Commands should be concatenated, no whitespace, terminate with Linefeed (0x0A).

## Example

Set the first 50 leds to rgb 50,50,50 -> rgb 0,0,0. 

```
R0:50J0L!0:!0,!0,!0U1W10S0:1JZ0:1JA0J1
```
### Explanation...
Eventually, we'll have an editor to allow this to be written, and auto condensed to the above =)
```
//Example led pattern, light first 50 leds starting at r=50,g=50,b=50, decreasing to r=0,g=0,b=0
R0:50             //set R0 to 50
J0                //mark jump label 0
L!0:!0,!0,!0      //set led in r0 to rgb using r0,r0,r0
U1                //push updates to leds
W10               //wait 10 units
S0:1              //decrement r0 by 1
JZ0:1             //if r0 is 0, jump to label 1
JA0               //unconditional jump to label 0
J1                //mark label 1
```
