#!/bin/bash

# NOTE: Needs to be absolute paths!
declare catkin_ws_dir="/home/$USER/Dev/catkin_ws"
declare px4_dir="/home/$USER/Dev/PX4/PX4_2017"

#source '$(px4_dir)'/Tools/setup_gazebo.bash '$(px4_dir)' '$(px4_dir)'/build_posix_sitl_default

# Get library files
cp $catkin_ws_dir/devel/lib/libiarc_obstacle.so $catkin_ws_dir/src/control_obstacle_sim_gazebo/plugins

source $px4_dir/Tools/setup_gazebo.bash $px4_dir $px4_dir/build_posix_sitl_default/
GAZEBO_MODEL_PATH=$catkin_ws_dir/src/control_obstacle_sim_gazebo/models:$GAZEBO_MODEL_PATH
GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:$px4_dir/Tools/sitl_gazebo/models
GAZEBO_PLUGIN_PATH=$GAZEBO_PLUGIN_PATH:$catkin_ws_dir/src/control_obstacle_sim_gazebo/plugins
GAZEBO_PLUGIN_PATH=$GAZEBO_PLUGIN_PATH:$px4_dir/build_posix_sitl_default/build_gazebo

unset catkin_ws_dir
unset px4_dir

