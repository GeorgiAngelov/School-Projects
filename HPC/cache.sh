#! /bin/bash

IFS=$'\n'
levels=($(cat /sys/devices/system/cpu/cpu0/cache/index?/level))
size=($(cat /sys/devices/system/cpu/cpu0/cache/index?/size))
types=($(cat /sys/devices/system/cpu/cpu0/cache/index?/type))
lineSize=($(cat /sys/devices/system/cpu/cpu0/cache/index?/coherency_line_size))

for ((i=0;i<$((${#levels[*]}));i++)); do
  echo "L${levels[$i]} ${types[$i]}, size (Bytes): ${size[$i]}, Cache line Size (Bytes): ${lineSize[$i]}"
done
