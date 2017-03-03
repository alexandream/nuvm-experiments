#!/bin/bash

make -f Makefile | grep -v -E "^make\[[0-9]+\]: (Entering|Leaving)"
