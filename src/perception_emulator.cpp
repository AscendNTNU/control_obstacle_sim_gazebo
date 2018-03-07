#include <ros/ros.h>
#include <tf/transform_datatypes.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <cmath>
#include <vector>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Point32.h>
//#include <geometry_msgs/Vector3.h>
#include <ascend_msgs/DetectedRobotsGlobalPositions.h>
#include <gazebo_msgs/ModelStates.h>

constexpr float PI{3.1415};
constexpr float lidar_scan_angle{270*PI/180}; // lidar scans a range of 270 degrees
constexpr bool lidar_sees_all{true};
ascend_msgs::DetectedRobotsGlobalPositions last_msg = {};

template<typename T>
inline T angleWrapper(const T angle){
    return angle - 2*PI * floor(angle / (2*PI));
}

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

bool isPrefix(const std::string& shorter, const std::string& longer){
    return std::equal(
            shorter.begin(),
            shorter.begin() + std::min(shorter.size(), longer.size()),
            longer.begin() );
}

void callback(const gazebo_msgs::ModelStates::ConstPtr model_states_p){
    using std::size_t;
    const size_t n_model_states = model_states_p->name.size();

    auto iris_state_p = std::find(model_states_p->name.cbegin(), model_states_p->name.cend(), "iris");

    bool limit_drone_perception = false;
    geometry_msgs::Point drone_pos;
    double drone_yaw = 0.0;
    
    if (limit_drone_perception){
        if (iris_state_p == model_states_p->name.cend()){
            ROS_WARN("Cant locate drone in gazebo/model_states");
        } else {
            int index = static_cast<int>(iris_state_p - model_states_p->name.cbegin());
            drone_pos = model_states_p->pose[index].position;
            auto quat = model_states_p->pose[index].orientation;
            tf2::Quaternion q(quat.x, quat.y, quat.z, quat.w);

            tf2::Matrix3x3 mat(q);
            double roll, pitch; // not used
            mat.getRPY(roll, pitch, drone_yaw);
        }
    }

    int obstacle_counter = 0;
    ascend_msgs::DetectedRobotsGlobalPositions msg;
    std::vector<geometry_msgs::Point32> position;
    std::vector<float> position_probability;
    std::vector<uint8_t> robot_color;
    std::vector<float> direction;
    std::vector<float> direction_probability;
    for (size_t i = 0; i < n_model_states; ++i){
        if (isPrefix("iarc_obstacle", model_states_p->name[i])){
            
            geometry_msgs::Point obs_pos = model_states_p->pose[i].position;
	    // check if obstacle is visable
            if (limit_drone_perception){
                auto delta_x = obs_pos.x - drone_pos.x;
                auto delta_y = obs_pos.y - drone_pos.y;
                auto obstacle_angle = angleWrapper(std::atan2(delta_y, delta_x) - drone_yaw); // angle relative to drone

		if (lidar_scan_angle/2 < obstacle_angle && obstacle_angle < (2*PI - lidar_scan_angle/2)){
                    // obstacle not visible to lidar
                    continue;
		}
            }
            msg.global_robot_position.push_back(convertPointType<geometry_msgs::Point32>(obs_pos));
            msg.position_probability.push_back(1.f);

            msg.robot_color.push_back(0);

	    const auto vec = model_states_p->twist[i].linear;
            const float obs_direction = std::atan2(vec.y, vec.x);
	    msg.direction.push_back(obs_direction);
	    msg.direction_probability.push_back(1.f);

            obstacle_counter++;
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
