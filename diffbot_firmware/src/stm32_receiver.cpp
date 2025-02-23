#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/string.hpp>
#include <libserial/SerialPort.h>
#include <cstring>
class Stm32Receiver : public rclcpp::Node // MODIFY NAME
{
public:
    Stm32Receiver() : Node("stm32f4_receiver_node"), flag_(1) // MODIFY NAME
    {
        declare_parameter<std::string>("port", "/dev/serial/by-path/pci-0000\\:00\\:14.0-usb-0\\:2.3\\:1.0-port0");
        port_ = get_parameter("port").as_string();
        // sub_ = create_subscription<std_msgs::msg::String>("serial_transmit", 10 , std::bind(&Stm32Publisher::publishCMD, this, std::placeholders::_1));
        timer_ = create_wall_timer(std::chrono::seconds(1), std::bind(&Stm32Receiver::timerCallBack, this));
        stm32_.Open(port_);
        RCLCPP_INFO_STREAM(this->get_logger(), "Port is using: " << port_);
        stm32_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
        RCLCPP_INFO_STREAM(this->get_logger(), "The topic is ready");
    }
    ~Stm32Receiver()
    {
        RCLCPP_INFO_STREAM(this->get_logger(), "The deconstructor is called");
        stm32_.Close();
    }
 
private:
    std::string port_;
    uint8_t flag_;
    LibSerial::SerialPort stm32_;
    void timerCallBack()
    {   std::string result;
        RCLCPP_INFO_STREAM(this->get_logger(), "Timer call back is called.");

        try {
            if (rclcpp::ok() && stm32_.IsDataAvailable()) 
            {
            std::string result;
            stm32_.ReadLine(result,'\n',1000);
            // stm32_.FlushInputBuffer();
            uint8_t frame_data[14] = {0};
            std::memcpy((void *)frame_data, (void *)(result.data()), result.size());
            RCLCPP_INFO_STREAM(this->get_logger(), "Size of received data:  " << result.size());
            if (frame_data[0] == 'r')
            {
                RCLCPP_INFO_STREAM(this->get_logger(), "Detected v_right");
            }
            if (frame_data[6] == 'l')
            {
                RCLCPP_INFO_STREAM(this->get_logger(), "Detected v_left");                                   
            }
            float v_left = *((float *)(frame_data + 8));
            float v_right = *((float *)(frame_data + 2));
            // // Làm sạch chuỗi nhận được
            // result.erase(result.find_last_not_of("\r\n") + 1);

            if (!result.empty()) {
                RCLCPP_INFO_STREAM(this->get_logger(), "Data received: " << result);
                RCLCPP_INFO_STREAM(this->get_logger(), "V left: " << v_left << " , V right: " << v_right);


            } else {
                RCLCPP_WARN_STREAM(this->get_logger(), "Received an empty string.");
            }
            }
        } catch (const LibSerial::ReadTimeout&) {
                RCLCPP_INFO_STREAM(this->get_logger(), "Data received: " << result);
                RCLCPP_WARN_STREAM(this->get_logger(), "ReadLine timed out.");
        } catch (const std::exception &e) {
                RCLCPP_ERROR_STREAM(this->get_logger(), "Error reading from serial: " << e.what());
        }

    }
    rclcpp::TimerBase::SharedPtr timer_;
};
 
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Stm32Receiver>(); // MODIFY NAME
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}