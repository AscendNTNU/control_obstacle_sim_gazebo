#include <ros/ros.h>
#include <cmath>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Point32.h>
#include <ascend_msgs/DetectedRobotsGlobalPositions.h>
#include <gazebo_msgs/ModelStates.h>

ascend_msgs::DetectedRobotsGlobalPositions last_msg = {};
geometry_msgs::Point32 prev_position = {};

void callback(const gazebo_msgs::ModelStates::ConstPtr model_states_p){

    ascend_msgs::DetectedRobotsGlobalPositions msg = {};
    
    for (int i = 0; i < model_states_p->name.size(); i++){
	if (model_states_p->name[i] == "iarc_obstacle"){
            //ROS_INFO("%s", model_states->name[i].c_str());
	    
            geometry_msgs::Point current_pos_recv = model_states_p->pose[i].position;
            geometry_msgs::Point32 current_pos;
            current_pos.x = static_cast<float>(current_pos_recv.x);
            current_pos.y = static_cast<float>(current_pos_recv.y);
            current_pos.z = static_cast<float>(current_pos_recv.z);
		
	    auto diff_x = current_pos.x - prev_position.x;
	    auto diff_y = current_pos.y - prev_position.y;
	    float angle = std::atan2(diff_y, diff_x);

	    msg.header.stamp = ros::Time::now();
	    msg.header.stamp = ros::Time::now();
            msg.global_robot_position.push_back(current_pos);
            msg.direction.push_back(angle);
	    msg.count++;
            last_msg = msg;

            prev_position = current_pos;;
        }
    }

}

int main(int argc, char** argv){
    ros::init(argc, argv, "perception_emulator");

    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe<gazebo_msgs::ModelStates>("/gazebo/model_states",1, &callback);
    ros::Publisher pub = n.advertise<ascend_msgs::DetectedRobotsGlobalPositions>("/perception/obstacles/lidar", 1);

    ros::Rate rate(30.f);
    while (ros::ok()){
        pub.publish(last_msg);

        ros::spinOnce();
        rate.sleep();
    }
}
