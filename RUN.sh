#!/bin/bash

make

read -p "Press Enter to run..."

qemu-system-x86_64 -machine q35 -m 1G -cpu qemu64 -soundhw ac97 -drive file=disk.img -boot d -cdrom MaslOS.iso

