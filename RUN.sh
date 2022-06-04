#!/bin/bash

dir

cd kernel
rm -rf lib
cd ..

cd gnu-efi
make bootloader
cd ..


cd kernel
make kernel
make buildimg

read -p "Press Enter to run..."

make run

