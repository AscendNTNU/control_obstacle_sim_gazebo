#include <boost/bind.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/physics/Link.hh>
#include <gazebo/common/common.hh>

#include <ros/ros.h>
#include <geometry_msgs/Point.h>

#include <stdio.h>

constexpr float PI{3.1415};

namespace gazebo
{
    class Obstacle : public ModelPlugin
    {
        public: Obstacle () : ModelPlugin()
        {
            printf("Hello model!\n");

        }

        public: void Load(physics::ModelPtr _parent, sdf::ElementPtr _sdf)
        {

            this->start_time = common::Time::GetWallTime();
            
            this->model = _parent;

            this->updateConnection = event::Events::ConnectWorldUpdateBegin(
                    boost::bind(&Obstacle::OnUpdate, this, _1));

            printf("Model loaded\n");

            physics::LinkPtr linkptr = this->model->GetLink("link_11");

            if (linkptr != NULL){
                printf("Success\n");
            } else{
                printf("Failure\n");
            }

	    if (!ros::isInitialized()){
                int argc = 0;
                char **argv = NULL;
                ros::init(argc, argv, "gazebo_client",
                        ros::init_options::NoSigintHandler);
            }

            this->rosNode.reset(new ros::NodeHandle("gazebo_client"));

            this->rosPub = this->rosNode->advertise<geometry_msgs::Point>("obstacle_pos", 1);
            
        }

        public: void OnUpdate(const common::UpdateInfo& _info)
        {
            constexpr float vel{0.33f};
            constexpr float radius{5.f};
            constexpr float round_time{2*PI*radius/vel};
            common::Time t = common::Time::GetWallTime() - start_time;

	    const float vel_x = -vel*std::sin(2*PI*t.Double()/round_time);
	    const float vel_y =  vel*std::cos(2*PI*t.Double()/round_time);

            this->model->SetLinearVel(math::Vector3(vel_x, vel_y, 0));

            const gazebo::math::Pose pose = this->model->GetWorldPose();
            geometry_msgs::Point msg;
            msg.x = pose.pos.x;
            msg.y = pose.pos.y;
            msg.z = pose.pos.z;
            this->rosPub.publish(msg);

            //physics::LinkPtr linkptr = this->model->GetLink("ground_link");
	    //const float force_x = 10*std::cos(t.sec);
	    //const float force_y = 10*std::sin(t.sec);
            //linkptr->AddForce(math::Vector3(force_x, force_y, 0));

	    //gazebo::math::Vector3 v = pose.pos;
        }

	private: common::Time start_time;

        private: physics::ModelPtr model;

        private: event::ConnectionPtr updateConnection;

	private: std::unique_ptr<ros::NodeHandle> rosNode;

	private: ros::Publisher rosPub;
    };

    GZ_REGISTER_MODEL_PLUGIN(Obstacle)
}

