#!/bin/bash

rmmod phonebook_device
rmmod phonebook_module
make clean
make
insmod phonebook_module.ko
insmod phonebook_device.ko