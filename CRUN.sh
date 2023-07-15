#!/bin/bash

find . -type f -name '*.o' -delete
make clean

./RUN.sh

