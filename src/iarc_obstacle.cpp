#include <boost/bind.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/physics/Link.hh>
#include <gazebo/common/common.hh>
#include <sdf/sdf.hh>

#include <stdio.h>

constexpr float PI{3.1415};

namespace gazebo
{
    class Obstacle : public ModelPlugin
    {
        public: Obstacle () : ModelPlugin(){}

        public: void Load(physics::ModelPtr _parent, sdf::ElementPtr _sdf)
        {
	    //this->startPose = _sdf->GetElement("iarc_obstacle")->GetValuePose();

	    if (_sdf->HasElement("vel")){
		velocity = _sdf->Get<double>("vel");
                printf("[iarc_obstacle] Velocity %.2f registered\n", velocity);
	    }

            const gazebo::math::Pose pose = _parent->GetWorldPose();

            this->model = _parent;

	    this->start_pose = pose;

            this->updateConnection = event::Events::ConnectWorldUpdateBegin(
                    boost::bind(&Obstacle::OnUpdate, this, _1));


        }

        public: void OnUpdate(const common::UpdateInfo& _info)
        {
            constexpr auto vel{0.33};
            const gazebo::math::Pose pose = this->model->GetWorldPose();
            const auto radius = std::sqrt(std::pow(pose.pos.x - 10.0, 2) + std::pow(pose.pos.y - 10.0, 2));
            const auto angular_velocity = vel/radius;

	    const auto vel_x = -angular_velocity*(pose.pos.y - 10.0);
	    const auto vel_y =  angular_velocity*(pose.pos.x - 10.0);

            this->model->SetLinearVel(math::Vector3(vel_x, vel_y, 0));

        }

        private: physics::ModelPtr model;

        private: event::ConnectionPtr updateConnection;

	private: gazebo::math::Pose start_pose;

	private: double velocity = 0.33;
    };

    GZ_REGISTER_MODEL_PLUGIN(Obstacle)
}

