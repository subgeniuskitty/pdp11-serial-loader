##### Description

The programs contained in this directoy allow bootstrapping of a completely 
blank PDP-11 from an external host. It is not required that the PDP-11 have any 
pre-existing code in the form of drives or ROMs.

##### Compatibility

The client has been tested with a PDP-11/23+ (KDF11-B on SLU2) and a PDP-11/73
(KDJ11-B with DLV11-J). It should work with most PDP-11 CPUs and SLUs which are
compatible with the DLV11-J register layout.

The server has been tested with a PC running FreeBSD.

##### Instructions

These instructions assume your PDP-11 has a serial connection to a UNIX host, 
the bootstrap_server.c program has been compiled on the UNIX host, and the UNIX 
host has a binary image ready for transfer to the PDP-11.

Begin by loading the program contained in client.txt into the PDP-11 via the
frontpanel switches or ODT. Set the PDP-11's CPU registers as follows.

R0: The starting memory address at which the binary image should be stored.
R1: The number of bytes in the binary image. This information will be displayed
    by server.c immediately prior to transfer.
R2: The base (RCSR) register address of the SLU connected to the UNIX host.
R3: The address to which control should be transferred after the binary image 
    is transferred to the PDP-11.

Begin execution on the PDP-11. If the bootstrap program was loaded at the 
memory addresses listed in client.txt, begin execution at address 70000.

Initiate the transfer from the UNIX host with the command

    pdp11-serial-loader -i <binary> -o <port>

where <port> is the path to the device file for the serial port connected to the 
PDP-11 and <binary> is the path to the binary image file. For example, if using 
the first serial port under FreeBSD and a binary image named "pdp11.bin",
execute this command.

    pdp11-serial-loader -i pdp11.bin -o /dev/cuau0

In FreeBSD, add your user to the dialer group for permission to access the
serial port.

    pw groupmod dialer -m ataylor
    exec su -l ataylor

Once the binary image has transferred to the PDP-11, it will begin execution at 
the address contained in R3 which should be your code.

##### Binary Image Format

Since the bootstrap program must be manually toggled into the PDP-11, brevity 
is paramount. Thus, the binary image matches the little-endian byte ordering of 
the PDP-11 word.

Consider the following example program.

    CLR R0
    MOV R0 R1

This would correspond to a binary image with consecutive bytes of:

Byte | Value
===============
   0 | 00000000
   1 | 00001010
   2 | 00000001
   3 | 00010000

If loaded into memory starting at location 2000, the PDP-11's memory would look 
like this:

Address | Value
==========================
   2000 | 0000101000000000
   2002 | 0001000000000001
