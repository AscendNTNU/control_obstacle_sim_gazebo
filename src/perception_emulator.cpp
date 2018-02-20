#include <ros/ros.h>
#include <cmath>
#include <vector>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Point32.h>
#include <geometry_msgs/Vector3.h>
#include <ascend_msgs/DetectedRobotsGlobalPositions.h>
#include <gazebo_msgs/ModelStates.h>

ascend_msgs::DetectedRobotsGlobalPositions last_msg = {};

// Allows arbitrary conversions between point32, point, vector etc.
// Works on anything with x,y,z as members
template<typename T, typename K>
inline T convertPointType(const K& point){
    T new_point;
    new_point.x = point.x;
    new_point.y = point.y;
    new_point.z = point.z;

    return new_point;
}

void callback(const gazebo_msgs::ModelStates::ConstPtr model_states_p){
    using std::size_t;
    const size_t n_model_states = model_states_p->name.size();

    int obstacle_counter = 0;
    ascend_msgs::DetectedRobotsGlobalPositions msg;
    std::vector<geometry_msgs::Point32> position;
    std::vector<float> position_probability;
    std::vector<uint8_t> robot_color;
    std::vector<float> direction;
    std::vector<float> direction_probability;
    for (size_t i = 0; i < n_model_states; ++i){
        if (model_states_p->name[i] == "iarc_obstacle"){
            obstacle_counter++;
            
            geometry_msgs::Point gaz_pos = model_states_p->pose[i].position;
            msg.global_robot_position.push_back(convertPointType<geometry_msgs::Point32>(gaz_pos));
            msg.position_probability.push_back(1.f);

            msg.robot_color.push_back(0);

	    const geometry_msgs::Vector3 vec = model_states_p->twist[i].linear;
            const float obs_direction = std::atan2(vec.y, vec.x);
	    msg.direction.push_back(obs_direction);
	    msg.direction_probability.push_back(1.f);
        }
    }

    msg.header.stamp = ros::Time::now();
    msg.count = obstacle_counter;

    last_msg = msg; 
}

int main(int argc, char** argv){
    ros::init(argc, argv, "perception_emulator");

    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe<gazebo_msgs::ModelStates>("/gazebo/model_states",1, &callback);
    ros::Publisher pub = n.advertise<ascend_msgs::DetectedRobotsGlobalPositions>("/perception/obstacles/lidar", 1);

    ros::Rate rate(10.f);
    while (ros::ok()){
        ros::spinOnce();

        pub.publish(last_msg);
        rate.sleep();
    }
}
