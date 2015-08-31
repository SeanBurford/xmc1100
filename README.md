# xmc1100
Code for Infineon XMC1000 and XMC4000 series microcontrollers

src/ contains code to make use of the peripherals along with examples.
It is all gcc compatible and uses standard Makefiles.

I had some trouble working out the serial port details from the manual
so I disassembled the blinky app that comes with the board.  The result
is in disassembly/

docs/ contains useful documentation from Infineon and other sources.

scripts/ makes things easier, such as calculating serial port parameters.
