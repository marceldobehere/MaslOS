#!/bin/bash

if [ ! -f ./disk.img ]; then
    echo "<Creating disk.img>"
    fallocate -l 5242880 disk.img
fi

