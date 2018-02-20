#include <boost/bind.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/physics/Link.hh>
#include <gazebo/common/common.hh>

#include <stdio.h>

constexpr float PI{3.1415};

namespace gazebo
{
    class Obstacle : public ModelPlugin
    {
        public: Obstacle () : ModelPlugin(){}

        public: void Load(physics::ModelPtr _parent, sdf::ElementPtr _sdf)
        {
            printf("[custom] Model loaded!\n");
            
            this->model = _parent;

            this->updateConnection = event::Events::ConnectWorldUpdateBegin(
                    boost::bind(&Obstacle::OnUpdate, this, _1));
        }

        public: void OnUpdate(const common::UpdateInfo& _info)
        {
            constexpr auto vel{0.33};
            const gazebo::math::Pose pose = this->model->GetWorldPose();
            const auto radius = std::sqrt(std::pow(pose.pos.x, 2) + std::pow(pose.pos.y, 2));
            const auto angular_velocity = vel/radius;

	    const auto vel_x = -angular_velocity*pose.pos.y;
	    const auto vel_y =  angular_velocity*pose.pos.x;

            this->model->SetLinearVel(math::Vector3(vel_x, vel_y, 0));

        }

        private: physics::ModelPtr model;

        private: event::ConnectionPtr updateConnection;
    };

    GZ_REGISTER_MODEL_PLUGIN(Obstacle)
}

