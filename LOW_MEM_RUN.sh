#!/bin/bash
qemu-system-x86_64 -machine q35 -m 150M -cpu qemu64 -soundhw ac97 -drive file=disk.img -boot d -cdrom MaslOS.iso

