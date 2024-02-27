## Notice

Development on MaslOS has stopped and I am currently working on [MaslOS 2](https://github.com/marceldobehere/MaslOS-2). 

# MaslOS
an OS written in C++ and a bit of C. [Here](https://youtu.be/Y8eZsNkEQIs) is a little showcase of MaslOS.

The base was made following [Ponchos OS Dev Youtube Playlist](https://www.youtube.com/playlist?list=PLxN4E629pPnJxCQCLy7E0SQY_zuumOVyZ)

The OS is now using [limine](https://github.com/limine-bootloader/limine) as the bootloader.

![Thumbnail](/images/thumbnail.PNG "MaslOS running with some sample apps")


## Wiki and other Pages

Here is the [Masl OS Wiki](https://github.com/marceldobehere/MaslOS/wiki).

Here's the [Hackaday Page](https://hackaday.io/project/189063-maslos) for MaslOS.

Here's the [MaslOS Subreddit](https://www.reddit.com/r/maslos/).

Here are the [Terminal Commands](https://github.com/marceldobehere/MaslOS/wiki/Terminal).


## Features
* Display
  - VGA Output
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
  - Explorer
  - Image Viewer
  - RAM/File Hex Viewer
  - Notepad (Really Basic but works)
  - DOOM (Port from [here](https://github.com/Daivuk/PureDOOM). Mostly works though you can only run 1 instance of DOOM at the same time and relaunching is currently broken lol)
  - Music player
  - Magnifier
  - Paint (WIP but you can draw and stuff)
* Audio
  - PC Speaker 
  - AC97 Driver (Works in VM)
  - Custom Sound System (Which supports the AC97 Device and PC-Speaker and all devices I'll write drivers for)
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
  - RS232
  - AC97
  - PCI Serial Card
* Other Features
  - Stack Trace
  + Crash handling
    - Crash Window showing non fatal kernel panic / crash
    - Advanced Fatal General Protection Fault Handler and recoverer
    - Emergency Kernel Panic
    - DePaST (Debug Panic Serial Terminal)
  - 4 Crash Commands!
  - BF Interpreter
  - [MAAB Interpreter](https://github.com/marceldobehere/MAAL-Marcels-Amazing-Assembly-Language)
  - Experimental .o runner (Still majorly WIP)
  - KernelFiles
  - Auto Disk Stuff Detection
  + - Auto Disk Detection
    - Auto Partition Detection
    - Auto Filesystem Detection
  + [MRAPS/MRAFS To/From Windows FS Converter Tool](https://github.com/marceldobehere/Masl-Disk-File-Converter)
  + Serial Manager (Allows "networking" with a client connected to serial) (WIP)
    - [Windows Client](https://github.com/marceldobehere/MaslOS-CS-Serial-Client)
    - Custom "Serial"
    - Screen sharing
    - Audio sharing (TODO, Though idk how good it'll be at 10KB/S)
    - Remote Mouse/Keyboard input (TODO)
    - Possibly TCP/UDP Networking using the client computer as a proxy (DONE for tcp clients)
    - Network Drive of Client (TODO)


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
![Explorer Gif](/images/explorer%20yes.gif "Explorer Gif")
![Notepad Gif](/images/notepad%20yes.gif "Notepad Gif")
![Image Viewer](/images/image-component.PNG "Image Viewer")
![Magnifier](/images/magnifier1.PNG "Magnifier App")
![Music Player](/images/music%20player.PNG "Music Player App")
![Paint](/images/paint.PNG "Paint App")
![Pong running on real hardware](/images/pongus.jpg "Pong running on real hardware")
![Doom Port](/images/doom.png "DOOM 2 running on MaslOS")
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
