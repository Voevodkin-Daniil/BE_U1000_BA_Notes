# Description

USB host example.

It will read device descriptors from the attached device and print out
to terminal as follows:

```
Device 1: ID 058f:6387
Device Descriptor:
  bLength               18
  bDescriptorType       1
  bcdUSB                0200
  bDeviceClass          0
  bDeviceSubClass       0
  bDeviceProtocol       0
  bMaxPacketSize0       64
  idVendor              058f
  idProduct             6387
  bcdDevice             0106
  iManufacturer         1  Generic
  iProduct              2  Mass Storage Device
  iSerialNumber         3  G3XY7BSL
  bNumConfigurations    1

```

The example is intended to run on EVU-BA evaluation boards that feature
the USB Type C connector. For simplicity, Type C processing layer is excluded
out of the scope of this application. To make sure it operates properly one
needs a USB Type A to Type C hardware adapter attached to the board before
the microcontroller is powered up. It will force ID line of USB PHY to low
state and will assign the USB role to host. Then the user device may be
attached to USB A receptable of the adapter as usually.
