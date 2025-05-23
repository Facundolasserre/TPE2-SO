#!/bin/bash
# Set audio device variable !!

if [ "$(uname)" == "Darwin" ]; then
    audio="coreaudio"
else
    audio="pa"
fi
sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 
