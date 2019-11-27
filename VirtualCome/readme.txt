########################################################################
#
#                           VirtualCom.eww
#
# $Revision: 1591 $
#
########################################################################

DESCRIPTION
===========
  This example project shows how to use the IAR Embedded Workbench for ARM
 to develop code for IAR LPC2478-SK board.
  It implements USB CDC (Communication Device Class) device and installs
 it like a Virtual COM port. The UART0 is used for physical implementation
 of the RS232 port.
  With this example project, you can echo a character between the RS232 
 Physical interface and the Virtual Com Port USB interface. You will need to 
 open two separate (Tera Term or HyperTerm) windows. One for the virtual 
 interface and another for the UART. Set both sessions to 8-N-1 No flow control. 
 Make sure that the virtual com port occupies a port between 1-8. You may have 
 to change this in the Device Manager option under the advanced tab.
  
  Note:
	Before you can use the following example the proper .inf file needs to be 
 installed for your version of windows. Please look in this example project 
 folder a subfolder \VirCOM_Driver_"X"\ . In this you will see the proper .inf file 
 that must be installed when Windows tries to install the generic CDC class driver. 
 Please note that "X" denotes your particular operating system. If it does 
 not install properly, you may have to go into your device manager and look 
 under "other devices" and manually point the driver to the proper .inf file 
 for your operating system. When you have properly installed the driver you 
 will see an entry in your Device Manager|Ports that says IAR Virtual Com Port, 
 "your board"(where "your board is the particular NXP board you are using). 


COMPATIBILITY
=============
   The example project is compatible with the IAR LPC2478-SK
  evaluation board. By default, the project is configured to use the
  J-Link JTAG interface.

CONFIGURATION
=============
   After power-up the controller gets it's clock from internal RC oscillator that
  is unstable and may fail with J-Link auto detect, therefore adaptive clocking
  should always be used. The adaptive clock can be select from menu:
  Project->Options..., section Debugger->J-Link/J-Trace  JTAG Speed - Adaptive.

   Make sure that the following jumpers are correctly configured on the
  IAR LPC2478-SK evaluation board:

  Jumpers:
   EXT/JLINK  - depending of power source
   ISP_E      - unfilled
   RST_E      - unfilled
   BDS_E      - unfilled
   C/SC       - SC

  The LCD shares pins with Trace port. If ETM is enabled the LCD will not work.

GETTING STARTED
===============

  1) Start the IAR Embedded Workbench for ARM.

  2) Select File->Open->Workspace...
     Open the following workspace:

     <installation-root>\arm\examples\NXP\
     LPC24xx\IAR-LPC-2478\VirtualCom\VirtualCom.eww

  3) Run the program.

  4) Use a USB cable to connect the PC to the USB Device connector of the Board.
     The first time the device is connected to the computer, Windows will
     load the driver for identified device. The Virtual COM port driver is
     in the $PROJ_DIR$\VirCOM_Driver_XP\.