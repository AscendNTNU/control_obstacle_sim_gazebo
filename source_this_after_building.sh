#!/bin/bash

echo "Scanning for PX4"
declare px4_dirs=($(find /home/$USER -mindepth 1 -maxdepth 8 -type d -name "PX4_2017" 2> >(grep -v 'Permission denied' | grep -v '.cache')))

declare n_px4_dirs_found=$(wc -l <<< "$px4_dirs")

echo "Found $n_px4_dirs_found PX4_2017 directorie(s)"
let i=1
for px4_dir in $px4_dirs;
do
    echo "\t[$i] $px4_dir"
    let i++
done
unset i

let selected_px=-1
if [ $n_px4_dirs_found = 0 ]; then
    echo "No PX4 directories found"
elif [ $n_px4_dirs_found = 1 ]; then
    echo "Using the only PX4 directory found"
    let selected_px=1
else
    while [[ $selected_px < 1 || $selected_px > $n_px4_dirs_found ]]; do
        echo "Type the number [1..${n_px4_dirs_found}] corresponding to desired PX4 directory"
        read selected_px
    done
fi

let i=1
for px4_dir in $px4_dirs;
do
    if [[ $selected_px = $i ]]; then
        break
    fi;
    let i++
done
unset i

# If the above didnt work, comment it out and hardcode your px4 and catkin paths here
declare px4_ws_dir=$px4_dir
declare catkin_ws_dir="$(pwd)/../.."

# Get library files
cp $catkin_ws_dir/devel/lib/libiarc_obstacle.so $catkin_ws_dir/src/control_obstacle_sim_gazebo/plugins

source $px4_dir/Tools/setup_gazebo.bash $px4_dir $px4_dir/build_posix_sitl_default
declare GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:$(pwd)/models
declare GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:$px4_dir/Tools/sitl_gazebo/models
declare GAZEBO_PLUGIN_PATH=$GAZEBO_PLUGIN_PATH:$catkin_ws_dir/src/control_obstacle_sim_gazebo/plugins
declare GAZEBO_PLUGIN_PATH=$GAZEBO_PLUGIN_PATH:$px4_dir/build_posix_sitl_default/build_gazebo

unset catkin_ws_dir
unset n_px4_dirs_found
unset selected_px
unset px4_dirs
unset px4_dir

