### Description
QSPI usage example.  
This example allows to interact with development board's SPI flash memory IC (Micron MT25QL128aBA and ISSI IS25LP032D). Command line interface allows configure, get status, get the chip ID, reset memory IC, read, write and erase memory, enable/disable Quad mode and execute code from SPI flash memory (XIP mode).  
Connect host PC to the UART0 port (PORT0 pins 6,7) of the development board, run the serial terminal software (115200 8N1) and execute command "help" to get more info about available commands and parameters.


XIP mode must be supported with different flash memories. To do this, after the address phase (A23-A0), it is necessary to supplement the continuation phase of the XIP mode (M7-M0). 

CE#  \___________________________________________________________/
      0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
SCK  _|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_

      [       3-byte Address      ][  Mode  ][ 4 Dummy ][ Data 1 ][ Data 2 ]
IO0  --<A20><A16><A12><A8 ><A4 ><A0 ><M4 ><M0 >---zz---<D4 ><D0 ><D4 ><D0 >--
IO1  --<A21><A17><A13><A9 ><A5 ><A1 ><M5 ><M1 >---zz---<D5 ><D1 ><D5 ><D1 >--
IO2  --<A22><A18><A14><A10><A6 ><A2 ><M6 ><M2 >---zz---<D6 ><D2 ><D6 ><D2 >--
IO3  --<A23><A19><A15><A11><A7 ><A3 ><M7 ><M3 >---zz---<D7 ><D3 ><D7 ><D3 >--
      ^                   ^         ^           ^       ^

The XIP_CMD register is used for this purpose. An example of working with this register is shown using the IS25LP032D chip.