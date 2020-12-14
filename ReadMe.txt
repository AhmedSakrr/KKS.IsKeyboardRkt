##
IOAPIC = I/O Advanced Programmable Interrupt Controller

Input  Source     Function/Connected Pin
1      Keyboard   IRQ1 using SERIRQ 

##
IOTRn = I/O Trap Registers.

Chipset configuration registers are mapped into memory space
at address RCBA (Root Complex Base Address Register). RCBA is 
gathered from the PCI-to-LPC bridge.

RCBA - Root Complex Base Address Register (LPC I/F-D31:F0)

Offset     Default Value   Attribute
F0-F3h     00000000h       R/W

Bit:
31:14 BaseAddress (BA), R/W
13:1  Reserved
0     Enable (EN), R/W. When set, this bit enables the range 
      specified in BA to be claimed as the Root Complex Register Block.

Interesting Chipset Registeres in this application is:

Offset      IOTRn                     Default Value     Access
1E80h–1E87h IOTR0 I/O Trap Register 0 0000000000000000h R/W 
1E88h–1E8Fh IOTR1 I/O Trap Register 1 0000000000000000h R/W 
1E90h–1E97h IOTR2 I/O Trap Register 2 0000000000000000h R/W 
1E98h–1E9Fh IOTR3 I/O Trap Register 3 0000000000000000h R/W 