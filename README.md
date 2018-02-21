# control_obstacle_sim_gazebo
Drone and obstacle simulation in gazebo

# Guide
1) Clone the repo into catkin_ws/src
2) Build your catkin_ws (not tested with catkin build)
3) Go into this repos folder and edit file "source_this_after_build.sh"
    * Change the _px4_dir_ to match the PX4_2017 folder (Must be an absolute path, for instance /home/$USER/PX4_2017)
4) execute _source source_this_after_building.sh_
5) Prepare yourself
6) Open three (or four) terminal windows
    6.1. In the one that you sourced the file "source\_this\_after\_building.sh", execute _roslaunch control\_obstacle\_sim\_gazebo mist.launch_. This launches the gazebo world and perception emulator and is the only one necessary if you just want to view the world. If you want control over the drone, keep reading. 
    6.2. In another terminal, cd to the PX4\_2017 folder and execute "no\_sim=1 make posix\_sitl\_default gazebo". This launches sets up sitl simulation of PX4 firmware for gazebo. The no\_sim=1 indicates that the command shouldn't launch gazebo. This window is where the commander app lies, which allows low level control of drone
    6.3 In another terminal, launch control\_fsm (for instance with _roslaunch control\_fsm test\_control\_fsm\_sim.launch_)
7) Your terminal window should now look something like this 
![](should\ look\ something\ like\ this.png)



