#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/string.hpp>
#include <libserial/SerialPort.h>
#include <string.h>
#include <string>

class Stm32Publisher : public rclcpp::Node // MODIFY NAME
{
public:
    Stm32Publisher() : Node("stm32f4_publisher_node"), flag_(1) // MODIFY NAME
    {
        declare_parameter<std::string>("port", "/dev/ttyUSB0");
        port_ = get_parameter("port").as_string();
        // sub_ = create_subscription<std_msgs::msg::String>("serial_transmit", 10 , std::bind(&Stm32Publisher::publishCMD, this, std::placeholders::_1));
        timer_ = create_wall_timer(std::chrono::seconds(2), std::bind(&Stm32Publisher::timerCallBack, this));
        stm32_.Open(port_);
        stm32_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
        RCLCPP_INFO_STREAM(this->get_logger(), "The topic is ready");
    }
    ~Stm32Publisher()
    {
        RCLCPP_INFO_STREAM(this->get_logger(), "The deconstructor is called");
        stm32_.Close();
    }
 
private:
    std::string port_;
    // rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    uint8_t flag_;
    LibSerial::SerialPort stm32_;
    void publishCMD(const std_msgs::msg::String &msg)
    {
        RCLCPP_INFO_STREAM(this->get_logger(), "New message received that will be published on serial: " << msg.data);
        stm32_.Write(msg.data);
    }
    void timerCallBack()
    {
        RCLCPP_INFO_STREAM(this->get_logger(), "Timer call back is called.");
        // if(flag_ == 1)
        // {
        //     RCLCPP_INFO_STREAM(this->get_logger(), "Turn on led");
        //     stm32_.Write("1");
        //     flag_ = 0;
        // }else if (flag_ == 0)
        // {
        //     RCLCPP_INFO_STREAM(this->get_logger(), "Turn off led");
        //     stm32_.Write("0");
        //     flag_ = 1;
        // }
        /*Test frame uart*/
        float v_left = 1.12;
        float v_right = 1.12;
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
        std::string frame_str(reinterpret_cast<const char *>(frame_s), sizeof(frame_s));
        RCLCPP_INFO_STREAM(this->get_logger(), frame_str);
        stm32_.Write(frame_str);
    }
    rclcpp::TimerBase::SharedPtr timer_;
};
 
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Stm32Publisher>(); // MODIFY NAME
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}