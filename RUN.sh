#!/bin/bash

dir

cd gnu-efi
make bootloader
cd ..


cd kernel
make kernel
make buildimg

read -p "Press Enter to run..."

make run

