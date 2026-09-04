### Description
USB DFU class example.
Create USB device of class DFU.
```bash
    $ lsusb
    ...
    Bus <bus_num> Device <dev_num>: ID 600d:1705 BE TinyUSB BE MCU TinyUSB Device
    ...
```
Allows to load binary files and store them in TCM, eFlash or QSPI flash memory.
The debug information is printed to UART0.
As DFU upload memory regions include TCMB and eFlash, this example is intended
to be run from TCMA.  
DFU download speed (USB High-speed): ~30 kB/s  
DFU download speed significantly depends on the selected USB speed mode (Full-speed or High-speed, see makefilt) and the DFU transfer buffer size (see CFG_TUD_DFU_XFER_BUFSIZE in the tusb_config.h file).  
***
### Settings
Use **dfu-util** command line tool on the host PC for DFU communications.
```bash
    # List currently attached DFU capable devices
    $ dfu-util -l
    Found DFU: [600d:1705] ver=0100, devnum=66, cfg=1, intf=0, path="1-1", alt=2, name="QSPI", serial="0123456789ABCDEF"
    Found DFU: [600d:1705] ver=0100, devnum=66, cfg=1, intf=0, path="1-1", alt=1, name="eFlash", serial="0123456789ABCDEF"
    Found DFU: [600d:1705] ver=0100, devnum=66, cfg=1, intf=0, path="1-1", alt=0, name="TCM", serial="0123456789ABCDEF"
```
