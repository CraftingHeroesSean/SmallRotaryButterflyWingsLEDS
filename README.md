# SmallRotaryButterflyWingsLEDS
RGBWLED-Programming

Circuitous Flight Butterfly Documentation: 
Thank you for adopting Circuitous Flight. I hope it will bring your environment a little color and light. 

Magnetic Wings: 
The butterfly look can be adapted using magnetic wings. These wings have a channel that is approximately the depth of the outline of the butterfly, however the channel is slightly taller than the butterfly to allow for small adjustments. To swap, simply take them off and replace with alternative wings or remove completely for a simpler look. 

RGBW LED Wings: 
The black wings provided have four parts, the wings themselves, the LED light strip inside the wings, a magnetic wing backing, and optional magnetic tails. If the wings are going to be used, for safety and to ensure that the LED strip stays in place, please be sure to attach the magnetic wing backs.
To remove these wings, it is best to remove the backs and tails (if attached) and to fold the top side of the wings towards each other until touching. Reattach the wing backs (and tails if desired), and then unwind the wings’ power cord running from the base until it is free of the stem of the butterfly. 

LED Base: 
Button	ProgramS
Power -	Turn off Lights
0	    - Natural White
1	    - Cool White
2	    - Red 
3    	- Orange
4	    - Yellow
5	    - Green
6	    - Blue
7	    - Pink
8	    - Marquee White
9	    - Marquee Rainbow
Loop	- Rainbow 
|<<	  - Previous Color Rainbow 
|>>	  - Next Color Rainbow

Programs 8, 9, and Loop must complete before the IR remote can pass in another command. 

The base of the butterfly has three pieces: the top lid, which is loose; the bottom of the base that contains the electronics; a drawer for a remote to control the LEDs. The drawer includes the graphic on the right showing which programs are available in the default configuration. The programs for buttons 8, 9, and Loop are animations, and until the animations stop a new command can’t be entered. Turning off and on the butterfly will stop the animation if another light program is desired. 

For Developers: 
There is an Inland ESP32-WROOM-32D Module board from Microcenter controlling the lighting. A Micro USB can be used to send new programs to the board if desired and the included programming sketches can be downloaded from GitHub. The program currently includes the NeoPixel library from Adafruit Industries and the IRRemote library from Armin Joachimsmeyer. The Arduino IDE was used to create and pass information to the ESP32 board.
