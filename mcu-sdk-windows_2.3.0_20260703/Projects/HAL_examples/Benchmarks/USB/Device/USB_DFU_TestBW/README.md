# Description

This example is a USB benchmark based on USB DFU class example. It is used to test
speed of DFU file transfers. It allows to download/upload binary files to/from TCM,
eFlash or QSPI flash memory with the use of dfu-util. Compared to the USB DFU example,
the sizes of TCM and eFlash memory regions are virtually extended to 16MB. This allows
to increase execution time and precision of the measurements. When the current read/write
address reaches the last address of the memory region, the next read/write operation
starts at the first address.

The benchmark does not assume that the file being written is the actual firmware,
so any file with random data may be used. TCM and eFlash memory regions include
all available memory space and limited only by capability of the microcontroller.

The benchmark should run from TCM when eFlash being tested. And vice versa, it
should run from eFlash while performing operations in TCM.

# Commands

1. List currently attached DFU capable devices:

```
dfu-util -l
```

2. Download a file to the device:

```
dfu-util -a 0 -D ~/Downloads/data.bin
```

3. Upload a file from the device:

```
dfu-util -a 0 -U ~/Downloads/data.bin
```

In Linux OS, the time utility may be used to test execution time:
```
time dfu-util -a 0 -D ~/Downloads/data.bin
```
