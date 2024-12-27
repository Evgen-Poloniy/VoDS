#!/bin/bash

num=5
count=0

steps=("vulkan-tools" "libvulkan-dev" "libglfw3-dev" "libglm-dev" "libxxf86vm-dev libxi-dev")

for step in "${steps[@]}"; do
  sudo apt remove -y $step
  let "count += 1"
  echo "$count / $num steps have been removed: $step"
done

if [[ $? ]]
then
    echo "All packeges have removed"
else
    echo "Some packeges have not removed by unknown error"
    echo 'Also, you may visit: https://vulkan-tutorial.com/Development_environment#page_Linux'
fi