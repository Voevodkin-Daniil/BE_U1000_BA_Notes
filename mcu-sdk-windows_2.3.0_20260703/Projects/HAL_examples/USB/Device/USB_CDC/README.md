### Description
USB CDC class example.  
Create USB device of class CDC.  
Echo back all received characters.  
```bash
    $ dmesg
    ...
    usb 1-1: new full-speed USB device number 67 using xhci_hcd
    usb 1-1: New USB device found, idVendor=600d, idProduct=1705, bcdDevice= 1.00
    usb 1-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
    usb 1-1: Product: TinyUSB Device
    usb 1-1: Manufacturer: TinyUSB
    usb 1-1: SerialNumber: 0123456789ABCDEF
    cdc_acm 1-1:1.0: ttyACM0: USB ACM device
```
The debug information is printed to the debug UART.
***
### Settings
For some Linux distributions it is necessary to configure the TTY device for proper "cat" command execution:
```bash
    # Sender terminal
    # USB CDC device name is mostly either USB0 or ACM0
    $ stty raw -echo -F /dev/tty<dev_name>
    $ echo "Hello, world!" > /dev/tty<dev_name>
```
```bash
    # Receiver terminal
    $ cat /dev/tty<dev_name>
    $ Hello, world!
```
