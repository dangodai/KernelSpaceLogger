# remote-keylogger-lkm

## Usage
Run the kernel module on the machine to be logged from. Use the python client to receive the key presses remotely. 

###Kernel Module
Modify settings.h before building
To build and insert the module: 
```bash
$ make
# insmod keylogger.ko
```
Make sure you have the linux headers installed on your system or it will fail to compile.

###Receiving Client
Modify the address/port to bind at the top of the file, run to start listening: 
```bash
$ python listener.py
```
