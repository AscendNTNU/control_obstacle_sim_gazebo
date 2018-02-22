#!/bin/bash

echo "Scanning for PX4"
declare px4_dirs=($(find / -mindepth 1 -maxdepth 8 -type d -name "PX4*" 2> >(grep -v 'Permission denied' | grep -v '.cache')))
echo "Scanning for catkin_ws"
declare cat_dirs=($(find / -mindepth 1 -maxdepth 8 -type d -name "*_ws" 2> >(grep -v 'Permission denied' | grep -v '.cache')))

#echo $px4_dirs

declare n_px4_dirs_found=$(wc -l <<< "$px4_dirs")
declare n_cat_dirs_found=$(wc -l <<< "$cat_dirs")

declare user_confirm_px4='n'
declare user_confirm_catkin='n'

echo "Found $n_px4_dirs_found PX4_2017 directorie(s)"
let i=1
for px4_dir in $px4_dirs;
do
    echo "\t[$i] $px4_dir"
    let i++
done

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

echo "Found $n_cat_dirs_found PX4_2017 directorie(s)"
let i=1
for cat_dir in $cat_dirs;
do
    echo "\t[$i] $cat_dir"
    let i++
done

let selected_cw=-1
if [ $n_cat_dirs_found = 0 ]; then
    echo "No catkin_ws directories found"
elif [ $n_cat_dirs_found = 1 ]; then
    echo "Using the only catkin_ws found"
    let selected_cw=1
else
    while [[ $selected_cw < 1 || $selected_cw > $n_cat_dirs_found ]]; do
        echo "Type the number [1..${n_cat_dirs_found}] corresponding to desired catkin_ws"
        read selected_cw
    done
fi

let i=1
for cat_dir in $cat_dirs;
do
    if [[ $selected_cw = $i ]]; then
        break
    fi;
    let i++
done

echo "------------------ using -----------------\n\t$px4_dir\n\t$cat_dir"

# If the above didnt work, comment it out and hardcode your px4 and catkin paths here
declare px4_ws_dir=$px4_dir
declare catkin_ws_dir=$cat_dir

# Get library files
cp $catkin_ws_dir/devel/lib/libiarc_obstacle.so $catkin_ws_dir/src/control_obstacle_sim_gazebo/plugins

source $px4_dir/Tools/setup_gazebo.bash $px4_dir $px4_dir/build_posix_sitl_default
declare GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:$(pwd)/models
declare GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:$px4_dir/Tools/sitl_gazebo/models
declare GAZEBO_PLUGIN_PATH=$GAZEBO_PLUGIN_PATH:$catkin_ws_dir/src/control_obstacle_sim_gazebo/plugins
declare GAZEBO_PLUGIN_PATH=$GAZEBO_PLUGIN_PATH:$px4_dir/build_posix_sitl_default/build_gazebo

unset catkin_ws_dir
unset px4_dir

