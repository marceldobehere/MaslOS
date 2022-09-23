# MaslOS
an OS written in C++ and a bit of C.
The base was made following [Ponchos OS Dev Youtube Playlist](https://www.youtube.com/playlist?list=PLxN4E629pPnJxCQCLy7E0SQY_zuumOVyZ)


MaslOS is 1000% the best os ever!11!1



## Features
* Display
  - VGA Output with GOP
* Desktop
  - Background Image
  - Taskbar
* Window Manager
  - Working Window Manager with dynamic windows
* Apps
  - Terminal
  - (Experimental) Text Display Window
* Audio
  - (In planning) Audio Device Driver of some sorts
* Keyboard
  - PS/2 Keyboard support
* Mouse
  - PS/2 Mouse support
* IO
  + Disk
    + Disk Interfaces
      - SATA / SATAPI
      - RAM Disk
      - File Disk
    + Partitioning Interfaces
      - MRAPS (Marcels really awesome partitioning system)
    + Filesystem Interfaces
      - MRAFS (Marcels really awesome filesystem)
* Memory
  - Dynamic Memory Management
  - (In planning) Paging
* OTHER IO
  - PIT
  - AHCI
  - ACPI
  - PCI
* Other Features
  - Stack Trace
  + Crash handling
    - Crash Window showing non fatal kernel panic / crash
    - Emergency Kernel Panic
  - Crash command
  - KernelFiles
  - Auto Disk Stuff Detection
  + - Auto Disk Detection
    - Auto Partition Detection
    - Auto Filesystem Detection


## How to run (Linux)
Install QEMU if you haven't.

Then download/clone the repo and run the `Just RUN OS.sh` file.





## How to run (Windows)
Install QEMU if you haven't.

Add QEMU To your PATH variable. [If ya have trouble installing it and don't know how to set the PATH variable, click here](https://linuxhint.com/qemu-windows/)

Then download/clone the repo and run the `Just RUN OS (Windows).cmd` file.





## How to build (Linux)
Install QEMU if you haven't.

install CMAKE if you haven't.

install NASM if you haven't.

(make sure mtools is on version 4.0.27 or lower. newer versions aren't working just yet)


Go into the MaslOS Folder and run the `RUN.sh` file



## Screenshots

![The MaslOS Splashscreen](/images/boot.PNG "The MaslOS Splashscreen")
![The Desktop with some windows open](/images/desktop.PNG "The Desktop with some windows open")
![The Desktop](/images/desktop%20background.PNG "The Desktop")
![Playing around with the Terminals](/images/terminal%20test%201.PNG "Playing around with the Terminals")
![A lot of windows](/images/yes.PNG "A lot of windows")
![The Panic screen](/images/filesystem%20test.PNG "A short test showing the filesystem working")
![The Panic screen](/images/crash%20window.PNG "The Crash window")
![The Panic screen](/images/new%20kernel%20panic.PNG "The new Panic screen")
[More Images](/images)
