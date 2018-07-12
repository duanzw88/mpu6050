#!/bin/sh
#该脚本为了确保i2c的驱动加载成功，并且设置i2c设备权限
sudo modprobe i2c-dev
sudo modprobe i2c-bcm2708
sleep 0.1
sudo chmod 666 /dev/i2c-1
