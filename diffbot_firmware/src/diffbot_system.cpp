#include "diffbot_firmware/diffbot_system.hpp"

/*That library defined hardware_interface::HW_IF_POSITION*/
#include <hardware_interface/types/hardware_interface_type_values.hpp> 
#include <rclcpp/rclcpp.hpp>

namespace diffbot_system
{
    DiffbotInterface::DiffbotInterface()
    {

    }

    DiffbotInterface::~DiffbotInterface()
    {
        if(stm32_.IsOpen())
        {
            try
            {
                /*Close the port*/
                stm32_.Close();
            }
            catch(...)
            {
                RCLCPP_FATAL_STREAM(rclcpp::get_logger("DiffbotInterface"), "Something went wrong while closing the connection with port " << port_);
            }
            
        }
    }

    CallBackReturn DiffbotInterface::on_init(const hardware_interface::HardwareInfo &hard_info)
    {
        CallBackReturn result = hardware_interface::SystemInterface::on_init(hard_info);
        if (result != CallBackReturn::SUCCESS)
        {
            return result;
        }

        try
        {
            port_ = info_.hardware_parameters.at("port");
        }
        catch(const std::out_of_range& e)
        {
            RCLCPP_FATAL(rclcpp::get_logger("DiffbotInterface"), "No Serial Port provied! Aborting");
            return CallBackReturn::FAILURE;
        }
        
        /*Allocate memory for container that save the info of hardware*/
        velocity_commands_.reserve(info_.joints.size());
        position_states_.reserve(info_.joints.size());
        velocity_states_.reserve(info_.joints.size());
        last_time_ = rclcpp::Clock().now();

        return CallBackReturn::SUCCESS;
    }

    std::vector<hardware_interface::StateInterface> DiffbotInterface::export_state_interfaces()
    {
        std::vector<hardware_interface::StateInterface> state_interfaces;
        for (size_t i = 0; i < info_.joints.size(); i++)
        {
            /*That code take the position state of robot*/
            state_interfaces.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, 
                hardware_interface::HW_IF_POSITION, &position_states_[i]));
            /*That code take the velocity state of robot*/
            state_interfaces.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, 
                hardware_interface::HW_IF_VELOCITY, &velocity_states_[i]));                        
        }
        return state_interfaces;
    }

    std::vector<hardware_interface::CommandInterface> DiffbotInterface::export_command_interfaces()
    {
        std::vector<hardware_interface::CommandInterface> command_interfaces;
        for (size_t i = 0; i < info_.joints.size(); i++)
        {
            command_interfaces.emplace_back(hardware_interface::CommandInterface(info_.joints[i].name, 
                hardware_interface::HW_IF_VELOCITY, &velocity_commands_[i]));
        }
        return command_interfaces;
    }

    CallBackReturn DiffbotInterface::on_activate(const rclcpp_lifecycle::State &)
    {
        RCLCPP_INFO(rclcpp::get_logger("DiffbotInterface"), "Statrting robot hardware...");
        velocity_commands_ = {0.0, 0.0};
        position_states_ = {0.0, 0.0};
        velocity_states_ = {0.0, 0.0};

        try
        {
            stm32_.Open(port_);
            stm32_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
        }
        catch(...)
        {
            RCLCPP_FATAL_STREAM(rclcpp::get_logger("DiffbotInterface"), "Something went wrong while establish port " << port_);
            return CallBackReturn::FAILURE;   
        }
        RCLCPP_INFO(rclcpp::get_logger("DiffbotInterface"), "Hardware started, ready to take commands");
        return CallBackReturn::SUCCESS;
    }

    CallBackReturn DiffbotInterface::on_deactivate(const rclcpp_lifecycle::State &)
    {
        RCLCPP_INFO(rclcpp::get_logger("DiffbotInterface"), "Stopping robot hardware...");
        if(stm32_.IsOpen())
        {
            try
            {
                stm32_.Close();
            }
            catch(...)
            {
                RCLCPP_FATAL_STREAM(rclcpp::get_logger("DiffbotInterface"), "Something went wrong while closing port " << port_);
                return CallBackReturn::FAILURE;                   
            }            
        }
        RCLCPP_INFO(rclcpp::get_logger("DiffbotInterface"), "Hardware stopped");
        return CallbackReturn::SUCCESS;
    }
    

    hardware_interface::return_type DiffbotInterface::read(const rclcpp::Time &, const rclcpp::Duration &)
    {
        try {
            if (rclcpp::ok() && stm32_.IsDataAvailable()) 
            {
                /*old method */
                // auto dt = (rclcpp::Clock().now() - last_time_).seconds();            
                // // stm32_.FlushInputBuffer();
                //     std::string message;
                //     stm32_.ReadLine(message, '\n',1000);
                //     // stm32_.FlushIOBuffers();

                //     // uint8_t frame_data[14] = {0};
                //     // std::memcpy((void *)frame_data, (void *)message.data(), message.size());
                //     // RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"), "raw received data: " << message);
                //     // RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"),"The amount of bytes are " << message.size());
                //     const uint8_t *frame_data = reinterpret_cast<const uint8_t *>(message.data());
                //     // RCLCPP_INFO(rclcpp::get_logger("DiffbotInterface"), "Data copy: %s",frame_data);
                //     if (!message.empty()) {
                //         // RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"), "Data received: " << message);
                //         float v_left;
                //         float v_right;
                //         int mutiplier = 1;
                //         if (frame_data[0] == 'r')
                //         {
                //             RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"), "Detected v_right");
                //             v_right = *((float *)(frame_data + 2));
                //             if(frame_data[1] == 'p')
                //             {
                //                 mutiplier = 1;
                //             }else if (frame_data[1] == 'n')
                //             {
                //                 mutiplier = -1;
                //             }else if (frame_data[1] == 'z')
                //             {
                //                 mutiplier = 1;
                //             }
                //             velocity_states_.at(0) = mutiplier * (double)v_right;
                //             position_states_.at(0) += velocity_states_.at(0) * dt; 
                //             RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"), "Data received: v_right: " << v_right);
                //         }
                //         if (frame_data[6] == 'l')
                //         {
                //             RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"), "Detected v_left");
                //             v_left = *((float *)(frame_data + 8));  
                //             if(frame_data[7] == 'p')
                //             {
                //                 mutiplier = 1;
                //             }else if (frame_data[7] == 'n')
                //             {
                //                 mutiplier = -1;
                //             }else if (frame_data[7] == 'z')
                //             {
                //                 mutiplier = 1;
                //             }
                //             velocity_states_.at(1) = mutiplier * (double)v_left;
                //             position_states_.at(1) += velocity_states_.at(1) * dt;   
                //             RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"), "Data received: v_left: " << v_left);                                   
                //         }
                //     } else {
                //         RCLCPP_WARN_STREAM(rclcpp::get_logger("DiffbotInterface"), "Received an empty string.");
                //     }
                /*----------------------------------------------------------------------------------------------*/
                /*new method*/
                std::string message;
                stm32_.ReadLine(message);
                if (!(message.empty()))
                {
                    auto dt = (rclcpp::Clock().now() - last_time_).seconds();   
                    std::stringstream ss(message);
                    RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"), "Received message: " << message);
                    std::string res;
                    int multiplier = 1;
           
                    while(std::getline(ss, res, ','))
                    {
                        multiplier = res.at(1) == 'p' ? 1 : -1;
                
                        if(res.at(0) == 'r')
                        {
                            velocity_states_.at(0) = multiplier * std::stod(res.substr(2, res.size()));
                            position_states_.at(0) += velocity_states_.at(0) * dt;
                        }
                        else if(res.at(0) == 'l')
                        {
                            velocity_states_.at(1) = multiplier * std::stod(res.substr(2, res.size()));
                            position_states_.at(1) += velocity_states_.at(1) * dt;
                        }
                    }
                    last_time_ = rclcpp::Clock().now();
                    RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"), "Data received: v_left: " << velocity_states_.at(1) << ", v_right: " << velocity_states_.at(0));   
                } else {
                    RCLCPP_WARN_STREAM(rclcpp::get_logger("DiffbotInterface"), "Received an empty string.");
                } 
            }
        } catch (const LibSerial::ReadTimeout&) {
                RCLCPP_WARN_STREAM(rclcpp::get_logger("DiffbotInterface"), "ReadLine timed out.");
        } catch (const std::exception &e) {
                RCLCPP_ERROR_STREAM(rclcpp::get_logger("DiffbotInterface"), "Error reading from serial: " << e.what());
        }

        /*Old method*/
        // if(rclcpp::ok && stm32_.IsDataAvailable())
        // {
        //     auto dt = (rclcpp::Clock().now() - last_time_).seconds();

        //     std::string message;
        //     stm32_.ReadLine(message);
        //     std::stringstream ss(message);
        //     std::string res;
        //     int multiplier = 1;
        //     while (std::getline(ss, res, ','))
        //     {
        //         if (res.at(1) == 'p')
        //         {
        //             multiplier = 1;
        //         }else 
        //         {
        //             multiplier = -1;
        //         }
        //         if (res.at(0) == 'r')
        //         {
        //             velocity_states_.at(0) = multiplier * std::stod(res.substr(2, res.size()));
        //             position_states_.at(0) += velocity_states_.at(0) * dt; 
        //         }else if (res.at(0) == 'l')
        //         {
        //             velocity_states_.at(1) = multiplier * std::stod(res.substr(2, res.size()));
        //             position_states_.at(1) += velocity_states_.at(1) * dt;
        //         }
        //     }
        //     last_time_ = rclcpp::Clock().now();
        // }
        return hardware_interface::return_type::OK;
    }

    hardware_interface::return_type DiffbotInterface::write(const rclcpp::Time &, const rclcpp::Duration &)
    {
        /*
            velocity_commands_.at(0): velocity of right wheel
            velocity_commands_.at(1): velocity of left wheel
        */
       /*Old method*/
        // std::stringstream message_stream;
        // char right_wheel_sign = velocity_commands_.at(0) >= 0 ? 'p' : 'n';
        // char left_wheel_sign = velocity_commands_.at(1) >= 0 ? 'p': 'n';
        // std::string compensate_zros_right = "";
        // std::string compensate_zero_left = "";

        // if(std::abs(velocity_commands_.at(0)) < 10.0)
        // {
        //     compensate_zros_right = "0";
        // }else 
        // {
        //     compensate_zros_right = "";
        // }

        // if (std::abs(velocity_commands_.at(1)) < 10.0)
        // {
        //     compensate_zero_left = "0";
        // }else 
        // {
        //     compensate_zero_left = "";
        // }
        // message_stream << std::fixed << std::setprecision(2) << "r" << right_wheel_sign << compensate_zros_right << std::abs(velocity_commands_.at(0)) << ",l" << left_wheel_sign << compensate_zero_left <<
        //     std::abs(velocity_commands_.at(1)) << ",";

        /*New method*/
        float v_right = (float)(velocity_commands_.at(0));
        float v_left = (float)(velocity_commands_.at(1));
        // RCLCPP_INFO_STREAM(rclcpp::get_logger("DiffbotInterface"), "Command velocity at write function: v_left: " << v_left << " , v_right: " << v_right);        
        /*create frame for sending*/
        uint8_t frame_s[14];
        frame_s[12] = '\r';
        frame_s[13] = '\n';
        frame_s[0] = 'r';
        if (v_right > 0)
        {
            frame_s[1] = 'p';
        }else 
        {
            frame_s[1] = 'n';
        }
        memcpy((void *)(frame_s + 2), (void *)&v_right, sizeof(float));
        frame_s[6] = 'l';
        if (v_left > 0)
        {
            frame_s[7] = 'p';
        }else 
        {
            frame_s[7] = 'n';
        }
        memcpy((void *)(frame_s + 8), (void *)&v_left, sizeof(float));
        std::string message(reinterpret_cast<const char *>(frame_s), sizeof(frame_s));

        try
        {
            // stm32_.FlushOutputBuffer();
            // stm32_.FlushIOBuffers();
            stm32_.Write(message);
        }
        catch(...)
        {
            RCLCPP_ERROR_STREAM(rclcpp::get_logger("DiffbotInterface"), "Something went wrong while sendind velocity command " << message << " on the port " << port_);
            return hardware_interface::return_type::ERROR;
        }
        return hardware_interface::return_type::OK;
    }
}

/*Pluggin it with ros2_control*/
#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(diffbot_system::DiffbotInterface, hardware_interface::SystemInterface)