# MaslOS
an OS written in C++ and a bit of C.
The base was made following [Ponchos OS Dev Youtube Playlist](https://www.youtube.com/playlist?list=PLxN4E629pPnJxCQCLy7E0SQY_zuumOVyZ)
The OS is now using [limine](https://github.com/limine-bootloader/limine) as the bootloader.

Here's the [Hackaday Page](https://hackaday.io/project/189063-maslos) for MaslOS.
Here's the [MaslOS Subreddit](https://www.reddit.com/r/maslos/).

Here is the [Wiki](https://github.com/marceldobehere/MaslOS/wiki).
Here are the [Terminal Commands](https://github.com/marceldobehere/MaslOS/wiki/Terminal).


## Features
* Display
  - VGA Output with GOP
* Desktop
  - Background Image
  - Taskbar
* Window Manager
  - Working Window Manager with dynamic windows
  - Start Menu Window
* GUI Framework
  - With support in MAAL!
* Apps
  - Terminal using a scrollable Text Window
  - Connect Four Game
  - Pong Game
  - Mandelbrot renderer
  - Explorer (WIP but basics work)
  - RAM Hex Viewer
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
    - Advanced Fatal General Protection Fault Handler and recoverer
    - Emergency Kernel Panic
  - 2 Crash Commands!
  - BF Interpreter
  - [MAAB Interpreter](https://github.com/marceldobehere/MAAL-Marcels-Amazing-Assembly-Language)
  - Experimental .o runner (Still majorly WIP)
  - KernelFiles
  - Auto Disk Stuff Detection
  + - Auto Disk Detection
    - Auto Partition Detection
    - Auto Filesystem Detection
  + [MRAPS/MRAFS To/From Windows FS Converter Tool](https://github.com/marceldobehere/Masl-Disk-File-Converter)


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
![Experimental Raytracer Demo](/images/raycaster%20demo.png "An experimental Raytracer Demo")
![Test Explorer Gif](/images/explorer%20test%201.gif "Test Explorer Gif")
![Pong running on real hardware](/images/pongus.jpg "Pong running on real hardware")
![MAAB Interpreter](/images/maab.png "MAAB Interpreter running with the windows interpreter next to it")
![The Desktop](/images/desktop%20background.PNG "The Desktop")
![Playing around with the Terminals](/images/terminal%20test%201.PNG "Playing around with the Terminals")
![A lot of windows](/images/yes.PNG "A lot of windows")
![Old Pong Gif](/images/pong%202.gif "Old Pong Gif")
![FS Test](/images/filesystem%20test.PNG "A short test showing the filesystem working")
![The Crash Window](/images/crash%20window.PNG "The Crash window")
![The Panic screen](/images/new%20kernel%20panic.PNG "The new Panic screen")

![Importing Windows Files](/images/importing%20text%20file.PNG "Importing Windows Files")
![Importing C# Project](/images/importing%20cs%20file.png "Importing C# Project")



[More Images](/images)
