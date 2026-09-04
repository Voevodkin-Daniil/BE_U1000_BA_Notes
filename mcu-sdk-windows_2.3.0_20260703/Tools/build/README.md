### Unified build system
This files are intended to be used as the unified firmware build system.  
The main goal is to simplify firmware building process: no need to edit linker script,
startup code and the main complex Makefile (common.mk).  
The template project could be used as a new project starting poin.  
Firmware build process:
* copy and rename <sdk_path>/Projects/template project
```bash
    $ cp -r <sdk_path>/Projects/_template <path>/<to>/<your>/<new>/<project>
```
* edit the source code of the new project
* edit Makefile to configure the new project (change the project name, add source
files or directories, change compiler options, add ne libraries, etc.)  
See `Makefile_template` file for more build options information.
* build the new project:
```bash
    $ make
    # building process info will appear here
```
* load the built firmware binary file to the MCU memory. See MCU documentation for
more info.
