#!/bin/bash
qemu-system-x86_64 --rtc base=localtime  -hda Image/x64BareBonesImage.qcow2 -m 512 -s -S -d int