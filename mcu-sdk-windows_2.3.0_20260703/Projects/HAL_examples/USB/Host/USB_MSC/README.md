# Description

USB host mass storage class example.

It demonstrates how to work with the attached USB flash drive. For proper
operation of the example the attached drive must have a single FAT formatted
partition.

The application searches for the file named readme.txt in the root folder
of the attached USB flash drive. If the file is found, the contents is
printed out to terminal. If not, the default file with this contents will be
created. Replace this text with you own data and re-attach USB drive to see it
in terminal.

The example is intended to run on EVU-BA evaluation boards that feature
the USB Type C connector. For simplicity, Type C processing layer is excluded
out of the scope of this application. To make sure it operates properly one
needs a USB Type A to Type C hardware adapter attached to the board before
the microcontroller is powered up. It will force ID line of USB PHY to low
state and will assign the USB role to host. Then the user device may be
attached to USB A receptable of the adapter as usually.
