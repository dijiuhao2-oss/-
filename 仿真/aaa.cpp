#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;//引入定时

class a1 : public rclcpp::Node
{
public:
    a1():Node("a1_node")
    {
        RCLCPP_INFO(this->get_logger(),"速度发布对象");
    
        publisher_ = this->create_publisher<std_msgs::msg::Int32>("/robot_speed",10);//话题、存入指针
        timer_ = this->create_wall_timer(1s,std::bind(&a1::on_timer,this));//定时
    }
private:
    void on_timer()
    {
        static std::vector<int> speeds = {2,5,4,8,3,6};//顺序速度数组
        static size_t index = 0;//数组下标

        auto message = std_msgs::msg::Int32();//实例化消息对象
        message.data = speeds[index];//速度加入队列

        RCLCPP_INFO(this->get_logger(),"发布速度:%d",message.data);
        publisher_->publish(message);//发布

        index = (index + 1) % speeds.size();//递增、循环
    }
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;//SharePtr 指针会自动销毁
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc , argv);

    rclcpp::spin(std::make_shared<a1>());
    
    rclcpp::shutdown();

    return 0;
}
