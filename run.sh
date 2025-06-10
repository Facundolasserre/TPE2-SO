#!/bin/bash


# Set audio device variable !!
if [[ "$1" = "gdb" ]]; then
    qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 -rtc base="2024-05-21T12:00:00" -d int
else
    qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -rtc base="2024-05-21T12:00:00" 
fi
