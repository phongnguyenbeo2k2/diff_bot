#ifndef DIFFBOT_SYSTEM_HPP
#define DIFFBOT_SYSTEM_HPP

#include <rclcpp/rclcpp.hpp>
#include <hardware_interface/system_interface.hpp>
#include <rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp>
#include <rclcpp_lifecycle/state.hpp>
#include <libserial/SerialPort.h>

namespace diffbot_system
{
    using CallBackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;
    class DiffbotInterface : public hardware_interface::SystemInterface
    {
    public:
        DiffbotInterface();
        virtual ~DiffbotInterface();
        
        virtual CallBackReturn on_activate(const rclcpp_lifecycle::State &previous_state) override;

        virtual CallBackReturn on_deactivate(const rclcpp_lifecycle::State &previous_state) override;

        virtual CallBackReturn on_init(const hardware_interface::HardwareInfo &hard_info) override;

        virtual std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

        virtual std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

        virtual hardware_interface::return_type read(const rclcpp::Time &time, const rclcpp::Duration &period) override;

        virtual hardware_interface::return_type write(const rclcpp::Time &time, const rclcpp::Duration &period) override;
    private:
        LibSerial::SerialPort stm32_;

        std::string port_;
        std::vector<double> velocity_commands_;
        std::vector<double> position_states_;
        std::vector<double> velocity_states_;
        rclcpp::Time last_time_;
    };
}
#endif