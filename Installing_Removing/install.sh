#!/bin/bash

num=5
count=0

steps=("vulkan-tools" "libvulkan-dev" "libglfw3-dev" "libglm-dev" "libxxf86vm-dev libxi-dev")

for step in "${steps[@]}"; do
  sudo apt install -y $step
  let "count += 1"
  echo "$count / $num steps have been installed: $step"
done

if [[ $? ]]
then
    echo "All packeges have installed. You may remove this packeges by running \"bash remove.sh\""
else
    echo "Some packeges have not installed by unknown error. If you want to remove installed packeges, you need to run \"bash remove.sh\""
    echo 'Also, you may visit: https://vulkan-tutorial.com/Development_environment#page_Linux'
fi