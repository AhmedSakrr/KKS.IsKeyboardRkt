# KKS.IsKeyboardRkt.2015
# Is Keyboard Rootkitted
License: **GPL-3.0-or-later**  
This application with its driver detects SMM rootkit on keyboard by checking I/O APIC IRQ1 and IOTR0-IOTR4 for
and SMI trapped keyboard handler. If it detects it, you have been SMM rootkitted.

Follow this (http://www.ikjetil.no/Home/Item/26) link to try it out. It should work, 
if not try booting without driver signature verfication.


