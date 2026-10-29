#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include <vector>//push_back()/size()/erase(begin())
#include <functional>//bing()
#include <queue>//push()/front()/pop()/size()/
#include <thread>//join()

template<typename T>
T absoluteDifference(T a,T b)
{
    return (a > b) ? (a - b) : (b - a);
}

class b1:public rclcpp::Node
{
public:
    b1() :Node("b1_node")//节点命名
    {
        sub = this->create_subscription<std_msgs::msg::Int32>(
            "/robot_speed",10,std::bind(&b1::do_cd,this,std::placeholders::_1)//话题、加入接收到的数据
        );
    }
private:

    //多线程
    void checkSpeedChange(int currentSpeed,int preivousSpeed)
    {
        int a = absoluteDifference(currentSpeed,preivousSpeed);

        if(a >= 3)
        {
            std::cout << "WARNING" << std::endl;
        }
        else
        {
            std::cout << "NORMAL" << std::endl;
        }
    }

    void do_cd(const std_msgs::msg::Int32 &msg)//接收
    {
        int current_speed = msg.data;//新速度

        std::cout << "\nReceive Speed: " << current_speed;

        speed_queue_.push(current_speed);//新速度排后面

        if(speed_queue_.size() < 2)//不足2个数据
        {
            std::cout << "\nWaiting for next command...\n";
            return;
        }

        int processed_speed = speed_queue_.front();//找出当前处理速度

        int speed_difference = absoluteDifference(processed_speed,previous_speed);//差值

        std::cout << "\nProcessing Speed: " << processed_speed 
        <<"\nPrivious Speed: "  << previous_speed 
        <<"\nSpeed Difference: " << speed_difference;


        previous_speed = processed_speed;

        std::cout << "\nState: ";
        //线程执行
        std::thread worker(&b1::checkSpeedChange,this,processed_speed,previous_speed);
        //等待线程结束、回收资源
        worker.join();

        history.push_back(speed_difference);//差值加入数组

        if(history.size() > 4)
        {
            history.erase(history.begin());
        }

        int state = 0;//最近差值中大于3的次数
        for(int val : history)
        {
            if(val >= 3)
            {
                state++;
            }
        }

        std::cout << "System State: ";

        if(state >= 2)
        {
            std::cout << "UNSTABLE\n";
        }
        else
        {
            std::cout << "STABLE\n";
        }

        speed_queue_.pop();//清老速度
    }
    
    std::vector<int> history;//最近4次速度变化
    int previous_speed = 0;//上次处理速度
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr sub;//保存数据
    std::queue<int> speed_queue_;//队列
};

int main(int argc, char const *argv[])
{
    rclcpp::init(argc , argv);
    rclcpp::spin(std::make_shared<b1>());
    rclcpp::shutdown();
    return 0;
}
