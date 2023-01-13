
summary
#cmakelists.txt学习
```cmake
cmake_minimum_required(VERSION 3.5) #设置cmake最低版本
project(ctest_task)#项目名称

#设置c++标准
# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#find package如果是系统提供的包直接使用find_package，如果不是系统直接提供的包，需要set（）需要包的头文件路径和库到变量里
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(mlog REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(test_msg REQUIRED)
find_package(Poco REQUIRED Net Util)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(PCL 1.9.0 REQUIRED)
find_package(carm_task_flow REQUIRED)

# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
set(carm_api_SHARED_PATH "/home/nixiaowei/carm_ws_ros2/install/")
#set（）carm_api的头文件和库到变量里
set(carm_api_INCLUDE_PATH ${carm_api_SHARED_PATH}carm_ros2/include/carm_api/api_wrapper/include)
set(carm_api_LIBRARIES_PATH ${carm_api_SHARED_PATH}carm_ros2/lib/libcarm_api.so)

set(carm_controller_INCLUDE_PATH "/home/nixiaowei/carm_ws_ros2/install/carm_controller/include/carm_common/include"
 "/home/nixiaowei/carm_ws_ros2/install/carm_controller/include/carm_communication/network_interface/websocket_client/include"
 "/home/nixiaowei/carm_ws_ros2/install/carm_controller/include/carm_communication/common/include")


set(ros_pub_node_INCLUDE_DIR "/home/nixiaowei/carm_ws_ros2/install/carm_deployer/include/com_module")

#include的目录，（）内是需要package内容里的头文件路径
include_directories(${carm_api_INCLUDE_PATH} 
${ros_pub_node_INCLUDE_DIR}

${carm_controller_INCLUDE_PATH} 
${PCL_INCLUDE_DIR}
#${pcl_conversions_INCLUDE_DIR}
# /opt/ros/foxy/include
${sensor_msg_INCLUDE_PATH})

#可执行选项设置：编译src目录下的main.cpp，生成ctest_task可执行文件
add_executable(ctest_task src/main.cpp)

#链接到库目录
target_link_libraries(ctest_task ${carm_api_LIBRARIES_PATH} ${Poco_LIBRARIES} ${terminal_helper_LIBRARIED_PATH} jsoncpp)

#添加执行文件所需要的依赖
ament_target_dependencies(ctest_task
pcl_conversions
rclcpp
mlog
test_msg
sensor_msgs)

#编译两个不同的.cpp文件需要分别设置所需要的库和依赖
add_executable(pub src/publish.cpp)
target_link_libraries(pub ${carm_api_LIBRARIES_PATH} ${Poco_LIBRARIES} ${terminal_helper_LIBRARIED_PATH}
${PCL_LIBRARIES}
jsoncpp)

ament_target_dependencies(pub
pcl_conversions
rclcpp
mlog
test_msg
sensor_msgs)

#安装
install(
  TARGETS
  ctest_task
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  INCLUDES DESTINATION include
)



ament_package()
```
#编译报错的一些常见问题
如果编译全是白色的报错，可执行文件找不到所需要的库
调用其他自己写的库是需要在install文件里导入所需要的库和头文件

***
sdk
对外提供接口的集合，可以供其他包调用sdk里的内容服务


内容：
1.尝试在carm_controller通讯层增加remote接口及websocket map
2.在sdk中增加对应接口，通过json协议与控制器remote接口通讯

#remote接口
```c++
string ControlSystemLoop::testMyRemoteApi(string str)
{
    minfo("controller testMyRemoteApi");
    Json::Value response_json;
    response_json["command"] = "myRemoteApi";
    response_json["operation"] = "get";
    response_json["id"] = 123;
    response_json["recv_msg"] = true;
    response_json["number"] = 3.14;
    return response_json.toStyledString();
}
websocketmap
websocket_handle_map_["myRemoteApi"] =
        bind(&ControlSystemLoop::testMyRemoteApi, this, std::placeholders::_1);
```

#定义键盘事件
```c++
keyboard_cmd.json
{
            "name": "myRemoteApi",
            "use_default": true,
            "arg": [
                "string",
                "int"
            ]
        }
        sdk中调用remote接口
         else if (cmd_name == "myRemoteApi")
        {
            minfo("this is test");
            std::string res = arm_ros_.getCarmApiCaller()->myRemoteApi(s_val[0], i_val[0]);}
```
            
#controller和sdk之间的通讯（消息回传）
sdk相当于客户端，controller相当于服务端
客户端：可以发送request和接收response
服务端：接收来自客户端的request，在服务端经过处理以后返回response
流程：
carm_sdk_generator调用sendCmdthreadFunc将api发送到sendcache里，然后调用parse（），将api里的string数据类型转换成json协议，调用sendCommand（）将json协议发送到服务端（sdk），json协议在sdk内的parsedate里进行解析，将json协议转换成string，controller上处理，再通过map绑定回调函数，remote接口的operation字段标为get，将remote接口里的数据通过websocketserver发送到websocketclient上接收到responsecache cache上，responsecache cache上的内容是通过remoteresponsecache上返回的

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#msg和srv定义
msg相当于一个结构体，里面存放用来发布数据的类型  
```c++
float64[6] joint_angles
int64 joint_lift
srv分为两部分，上部分是request内容，下部分是respose的内容，中间用---
string carm_ip
string extra_arm_ip
float64 delay_ms
int64 carm_port 
---
int64 ret
```
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#将回传的消息通过自定义的msg进行发布
先定义msg消息，msg消息需要在其他包上定义，将传回sdk的string类型转换成json类型，消息里的成员需要与json协议指定的内容相同，类型用asInt（），asBool（）去解析，定义发布节点，话题，publish（msg），publish里的是发布的内容
```c++
else if (cmd_name == "myRemoteApi")
        {
            minfo("this is test");
            std::string res = arm_ros_.getCarmApiCaller()->myRemoteApi(s_val[0], i_val[0]);
            Json::Value msg_json;
            Json::Reader().parse(res, msg_json);
            tutorial_interfaces::msg::Num msg;
            try
            {
                msg.num = msg_json["num"].asInt();
                msg.id = msg_json["id"].asInt();
                msg.recv_msg = msg_json["recv_msg"].asBool();
                msg.number = msg_json["number"].asFloat();
            }
            catch (Json::Exception &e)
            {
                minfo("catch exception -- %s", e.what());
            }
            auto node = make_shared<rclcpp::Node>("publish_node");//定义节点
            rclcpp::Publisher<tutorial_interfaces::msg::Num>::SharedPtr publisher_ = node->create_publisher<tutorial_interfaces::msg::Num>("mytopic", 10);//定义话题

            log_time::TimerUnit timer;
            timer_start(timer);
            timer_end(timer);
            while (timer.duration_ms < 5000)
            {
                publisher_->publish(msg);//发布msg
                timer_end(timer);
            }
        }
```
***
#通过srv消息将文件拷贝到指定目录
srv消息的定义
上半部分是读取文件的地址（string），下半部分是写入文件的地址
通过键盘事件调用remote的传输文件接口（transferfile），将任意json文件拷贝到指定目录
sdk将消息发到controller上，controller上去读取文件
controller只负责读取数据
```c++
string ControlSystemLoop::remotetransferFile(std::string str)
{
    minfo("%s", str.c_str());
    Json::Reader json_reader;
    Json::Value response_json;
    Json::Value request_json = carm_json::parseJsonWithCharReader(str);//将传入的str转换成request的json协议
    minfo("%s", request_json.toStyledString().c_str());

    std::string inpath = "";
    if (request_json.isMember("dir"))
    {
        inpath = request_json["dir"].asString();//将请求的dir解析成string赋给inpath
    }
    else
    {
        mwarn("miss param dir");//警告丢失路径参数
        return error_str(request_json["command"].asString(), response_json);//已经定义好的返回错误字符串
    }
    
    string txt_string;//将文件内容读入txt_string内
    minfo("dir name %s", inpath.c_str());
    boost::filesystem::path file_path(inpath);
    minfo("stop");
    if (boost::filesystem::exists(file_path))
    {
        string txt_name = file_path.string(); // + name;
        ifstream txt_file;
        txt_file.open(txt_name);
        stringstream strStream;
        strStream << txt_file.rdbuf();

        txt_string = strStream.str();
        txt_file.close();
    }
    else
    {
        mwarn("文件不存在！");
        return 0;
    }

    try
    {
        Json::Value file_json = carm_json::parseJsonWithCharReader(txt_string);
        response_json["txt"] = file_json;
        // std::cout << txt_string << std::endl;
    }
    catch (...)
    {
        return error_str(request_json["command"].asString(), response_json);
    }
    // response_json["command"] = "transferFile";
    response_json["operation"] = "get";

    // str = response_json.toStyledString();
    return recieved_str("transferFile", response_json);
}
```
#可以自定义读入文件路径和写入文件路径
```c++
std::string transferFile(std::string inpath)
    {
        Json::Value request_json;

        request_json["command"] = "transferFile";
        request_json["dir"] = inpath;//将inpath作为request_json发送到controller
        minfo("transferFile");
        std::string res = com_global.webSendTasks(request_json);

        log_time::TimerUnit timeout_timer;
        log_time::timer_start(timeout_timer);
        while (1)
        {
            log_time::timer_end(timeout_timer);
            if (timeout_timer.duration_ms > 3000)
            {
                mwarn("通讯超时...");
                return "";
            }
            ResponseCache cache = com_global.getResponseCache();
            if (cache.is_cache && cache.name == "transferFile")
            {
                try
                {
                    string txt_string;

                    minfo("recieve msg: %c%s", "blue", cache.detail.c_str());//回传回来的string类型都存储在cache.detail内，将cache.detail里的内容解析成res_json
                    Json::Value res_json;
                    if (Json::Reader().parse(cache.detail, res_json))
                    {
                        if (res_json.isMember("task"))
                        {
                            if (res_json["task"].isMember("txt"))
                            {
                                return res_json["task"]["txt"].toStyledString();
                            }
                        }
                    }
                    return cache.detail;
                }
                catch (...)
                {
                    std::cout << "catch error" << std::endl;
                }
            }
        }
    }
    
    carm_ros/main.cpp
     else if(cmd_name=="testtransferFile")
        {
            minfo("testtransferFile");
            std::string res=arm_ros_.getCarmApiCaller()->testtransferFile(s_val[0]);//只有一个string类型的参数
            minfo("%s",res.c_str());
            //write file to path将文件写入指定文件目录
            Json::StyledWriter sw;
            std::string save_dir=s_val[1];
            std::ofstream os;
            os.open(save_dir.c_str());
            if(os)
            {
                os<<res;
                os.close();
                minfo("save success,file %s",save_dir.c_str());
            }
            else
            {
                mwarn("can not open %s",save_dir.c_str());

        
            }
        }
```

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#内容
自己创建一个package，并对外提供服务，通过服务调用sdk里的接口，类似carm_ros2,相当于一个服务端，所需要的srv消息定义在其他包里，srv消息的定义与所需要sdk接口的参数是相对应的
将所需要sdk中的接口在自己定义的package内去做封装，封装在一个类里面，然后每个成员函数内去调用sdk里的接口
将msg的消息发布，msg包含六个机械壁节点和升降高度，将sensor_msgs内的机械臂关节值赋到自己定义的msgs内，然后将自己定义的msgs内的内容通过ros发布出来
```c++
#include "rclcpp/rclcpp.hpp"
#include "log_manager.h"
#include "carm_api.h"


#include "test_msg/srv/connect_carm.hpp"
#include "test_msg/srv/fast_to_joint_pos.hpp"
#include "test_msg/srv/set_controller_ready.hpp"
#include "test_msg/srv/set_debug.hpp"
#include "test_msg/srv/transfer_file.hpp"
#include "test_msg/msg/carm_joint_pose.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"


class Ros2Wrapper
{
private:
    // srv
    std::shared_ptr<rclcpp::Node> node;

    rclcpp::Service<test_msg::srv::ConnectCarm>::SharedPtr connectCarm_server;
    rclcpp::Service<test_msg::srv::FastToJointPos>::SharedPtr fastToJointPos_server;
    rclcpp::Service<test_msg::srv::SetControllerReady>::SharedPtr setControllerReady_server;
    rclcpp::Service<test_msg::srv::SetDebug>::SharedPtr setDebug_server;
    rclcpp::Service<test_msg::srv::TransferFile>::SharedPtr transferFile_server;
    rclcpp::Publisher<test_msg::msg::CarmJointPose>::SharedPtr carmJointPose_publish;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr sensorJointPose_publish;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr sensorJointPosetest_publish;
    
    CArmApiWrapper carm_api_caller;

    bool quit = false;
    std::thread thread_func;
    std::thread thread_publishCarmJointPose;
    std::thread thread_publishCarmJointPoseTest;
  


public:
    Ros2Wrapper()
    {
        node = std::make_shared<rclcpp::Node>("srv_node");

        connectCarm_server = node->create_service<test_msg::srv::ConnectCarm>(
            "connecttoarm",
            std::bind(&Ros2Wrapper::connect, this, std::placeholders::_1, std::placeholders::_2));

        fastToJointPos_server = node->create_service<test_msg::srv::FastToJointPos>(
            "fastToJointPos",
            std::bind(&Ros2Wrapper::fastToJointPos, this, std::placeholders::_1, std::placeholders::_2));

        setControllerReady_server = node->create_service<test_msg::srv::SetControllerReady>(
            "setControllerReady",
            std::bind(&Ros2Wrapper::setControllerReady, this, std::placeholders::_1, std::placeholders::_2));

        setDebug_server = node->create_service<test_msg::srv::SetDebug>(
            "setDebug",
            std::bind(&Ros2Wrapper::setDebug, this, std::placeholders::_1, std::placeholders::_2));
        transferFile_server = node->create_service<test_msg::srv::TransferFile>(
            "transferFile",
            std::bind(&Ros2Wrapper::transferFile, this, std::placeholders::_1, std::placeholders::_2));

        carmJointPose_publish = node->create_publisher<test_msg::msg::CarmJointPose>("carm_topic", 10);
        sensorJointPose_publish = node->create_publisher<sensor_msgs::msg::JointState>("carm_joint_pose", 10);
        sensorJointPosetest_publish=node->create_publisher<sensor_msgs::msg::JointState>("carm_joint_pose_test",10);
        thread_publishCarmJointPose = std::thread(std::bind(&Ros2Wrapper::publishCarmJointPose, this));
        thread_publishCarmJointPoseTest=std::thread(std::bind(&Ros2Wrapper::publishCarmJointPoseTest,this));
        
        
        thread_func = std::thread(std::bind(&Ros2Wrapper::threadrun, this));
        thread_func.join();
        thread_publishCarmJointPose.join();
        thread_publishCarmJointPoseTest.join();

      
    }

    ~Ros2Wrapper()
    {
        quit = true;
    }


    void threadrun()
    {
        minfo("thread start");
        while (true)
        {
            if (rclcpp::ok())
            {
                rclcpp::spin_some(node);
            }

            // if (carm_api_caller.is_connected())
            // {
            //     carm_api_caller.update_robot_state();
            // }
            // print_array("joint", carm_api_caller.joint_angles, 6);
            usleep(1000 * 100);
        }
    }

    bool connect(const test_msg::srv::ConnectCarm::Request::SharedPtr req,
                 test_msg::srv::ConnectCarm::Response::SharedPtr rsp)
    {
        minfo("connect test");
        std::string ip1 = "127.0.0.1";
        std::string ip2 = "127.0.0.1";
        int ret = carm_api_caller.connect_arm(req->carm_ip,
                                              req->extra_arm_ip,
                                              req->delay_ms);
        rsp->ret = ret;

        return true;
    }

    bool fastToJointPos(const test_msg::srv::FastToJointPos::Request::SharedPtr req,
                        test_msg::srv::FastToJointPos::Response::SharedPtr rsp)
    {
        minfo("FastToJointPose");
        std::array<double, 6> joint_pos{req->target_joint_pos[0], req->target_joint_pos[1], req->target_joint_pos[2],
                                        req->target_joint_pos[3], req->target_joint_pos[4], req->target_joint_pos[5]};
        // std::array<double,6> joint_pos;
        // for(int i = 0; i < 6; i++){
        //     joint_pos[i] = req->target_joint_pos[i];
        // }
        // std::copy_n(&req->target_joint_pos[2], 6, joint_pos.begin());
        Json::Value extra_request_json;
        Json::Reader().parse(req->extra_request, extra_request_json);
        std::string ret = carm_api_caller.fast_to_joint_pos(joint_pos,
                                                            req->speed,
                                                            req->priority,
                                                            extra_request_json);
        rsp->ret = ret;
        minfo("ret %s", rsp->ret.c_str());
        return true;
    }

    bool setControllerReady(const test_msg::srv::SetControllerReady::Request::SharedPtr req,
                            test_msg::srv::SetControllerReady::Response::SharedPtr rsp)
    {
        minfo("SetControllerReady");
        int ret = carm_api_caller.set_controller_ready(req->feedback, req->delay_ms);
        rsp->ret = ret;
        return true;
    }

    bool setDebug(const test_msg::srv::SetDebug::Request::SharedPtr req,
                  test_msg::srv::SetDebug::Response::SharedPtr rsp)
    {
        minfo("SetDebug");
        bool flag = true;
        int ret = carm_api_caller.set_debug(req->flag);
        rsp->ret = ret;
        return true;
    }

    bool transferFile(const test_msg::srv::TransferFile::Request::SharedPtr req,
                      test_msg::srv::TransferFile::Response::SharedPtr rsp)
    {
        minfo("TrasnferFile");

        std::string ret = carm_api_caller.transferFile(req->inpath);
        rsp->ret = ret;
        return true;
    }

    void publishCarmJointPose()
    {
        minfo("publishCarmJointPos");
        minfo("topic publish");
        while (true)
        {
            test_msg::msg::CarmJointPose rpose;
            sensor_msgs::msg::JointState jpose;
            if (carm_api_caller.is_connected())
            {
                carm_api_caller.update_robot_state();
                jpose.header.stamp = node->now();
                string s("joint_lift");
                jpose.name.push_back(s);
                for (int i = 0; i < 6; i++)
                {
                    string s("joint");
                    jpose.name.push_back(s.append(std::to_string(i + 1)));
                }
                jpose.position.push_back(0);
                //print_array("joint", carm_api_caller.joint_angles, 6);
                for (int i = 0; i < CARM_MAX_AXIS_NUM; i++)
                {
                    rpose.joint_angles[i] = carm_api_caller.joint_angles[i];

                    jpose.position.push_back(carm_api_caller.joint_angles[i]);
                }
                print_array("joint",jpose.position,7);
                carmJointPose_publish->publish(rpose);
                sensorJointPose_publish->publish(jpose);

                usleep(1000 * 100);
            }
        }
    }

    void publishCarmJointPoseTest()
    {
        minfo("test");
        while(true)
        {
            
            test_msg::msg::CarmJointPose robotState;
            sensor_msgs::msg::JointState jointState;
            if(carm_api_caller.is_connected())
            {
                carm_api_caller.update_robot_state();
                jointState.header.stamp=node->now();
                string str("joint_lift");
                jointState.name.push_back(str);
                for(int i=0;i<6;i++)
                {
                    string str("joint");
                    jointState.name.push_back(str.append(std::to_string(i+1)));
                }
                jointState.position.push_back(0);

                for(int i=0;i<CARM_MAX_AXIS_NUM;i++)
                {
                    robotState.joint_angles[i]=carm_api_caller.joint_angles[i];
                    jointState.position.push_back(carm_api_caller.joint_angles[i]);
                }
                print_array("jointtest",jointState.position,7);
                carmJointPose_publish->publish(robotState);
                sensorJointPose_publish->publish(jointState);
                
                usleep(1000*200);
            }
        }
    }

    
   
};

void sighandle(int sig)
{
    minfo("quit");
    if (sig == 2)
    { // ctrl + C
        rclcpp::shutdown();
        std::cout << "sig = " << sig << std::endl;
        raise(SIGQUIT);
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sighandle);
    mlog.setMsgBold(true);        // 设置变量粗体
    mlog.setTagBold(true);        // 设置tag粗体
    mlog.setLevelHighlight(true); // 设置等级粗体
    mlog.init(carm::LogManagerFlag::PRINT_IMMEDIATELY);

    rclcpp::init(argc, argv);

    Ros2Wrapper caller;

    rclcpp::shutdown();

    return 0;
}
```
#服务的定义格式
```c++
 bool setControllerReady(const test_msg::srv::SetControllerReady::Request::SharedPtr req,
                            test_msg::srv::SetControllerReady::Response::SharedPtr rsp)
    {
        minfo("SetControllerReady");
        int ret = carm_api_caller.set_controller_ready(req->feedback, req->delay_ms);
        rsp->ret = ret;
        return true;
    }
    
    如果是array类型的参数就需要一个一个赋值
bool fastToJointPos(const test_msg::srv::FastToJointPos::Request::SharedPtr req,
                        test_msg::srv::FastToJointPos::Response::SharedPtr rsp)
    {
        minfo("FastToJointPose");
        std::array<double, 6> joint_pos{req->target_joint_pos[0], req->target_joint_pos[1], req->target_joint_pos[2],
                                        req->target_joint_pos[3], req->target_joint_pos[4], req->target_joint_pos[5]};
        // std::array<double,6> joint_pos;
        // for(int i = 0; i < 6; i++){
        //     joint_pos[i] = req->target_joint_pos[i];
        // }
        // std::copy_n(&req->target_joint_pos[2], 6, joint_pos.begin());
        Json::Value extra_request_json;
        Json::Reader().parse(req->extra_request, extra_request_json);
        std::string ret = carm_api_caller.fast_to_joint_pos(joint_pos,
                                                            req->speed,
                                                            req->priority,
                                                            extra_request_json);
        rsp->ret = ret;
        minfo("ret %s", rsp->ret.c_str());
        return true;
    }
```
***
#如何写ros_client
将需要调用的api都封装到类里面，这个类需要继承iCommandBroadcast，创建ros_node节点，定义各种client
```c++
 std::shared_ptr<rclcpp::Node> ros_node;
    rclcpp::Client<test_msg::srv::ConnectCarm>::SharedPtr connect_carm_client;
    rclcpp::Client<test_msg::srv::FastToJointPos>::SharedPtr move_joint_client;
    rclcpp::Client<test_msg::srv::SetControllerReady>::SharedPtr set_ready_client;
    rclcpp::Client<test_msg::srv::SetDebug>::SharedPtr set_debug_client;
    rclcpp::Client<test_msg::srv::TransferFile>::SharedPtr transfer_file_client;

create_client
 ros_node = std::make_shared<rclcpp::Node>("ros_client");
        connect_carm_client = ros_node->create_client<test_msg::srv::ConnectCarm>("/connecttoarm");
        move_joint_client = ros_node->create_client<test_msg::srv::FastToJointPos>("/fastToJointPos");
        set_ready_client = ros_node->create_client<test_msg::srv::SetControllerReady>("/setControllerReady");
        set_debug_client = ros_node->create_client<test_msg::srv::SetDebug>("/setDebug");
        transfer_file_client = ros_node->create_client<test_msg::srv::TransferFile>("/transferFile");
        
获取到keyboard_cmd
```c++
  struct passwd* pwd;
        uid_t userid;
        userid = getuid();
        pwd = getpwuid(userid);
        std::string user_name = std::string(pwd->pw_name);

        std::string root_dir = "/home/" + user_name + "/carm_ws_ros2/src/ros_client/config/";
        std::string keyboard_cmd_file = root_dir + "keyboard_cmd.json";
        std::string keyboard_history_file = root_dir + "keyboard.history";
        keyboard_helper = std::make_shared<TerminalHelper>(true, keyboard_cmd_file, true, keyboard_history_file);
        keyboard_helper->attachCommandListener(this);
        // std::cout << "KeyboardHelper" << std::endl;
        keyboard_helper->broadcastAutomatic();
        
        调取命令
        if (cmd_name == "connect")
        {
            if (!connect_carm_client->wait_for_service(3s))
            {
                if (!rclcpp::ok())
                {
                    mwarn("服务等待中断");
                    return;
                }
                mwarn("服务等待超时");
                return;
            }
            auto request_msg = std::make_shared<test_msg::srv::ConnectCarm::Request>();

            request_msg->carm_ip = s_val[0];
            request_msg->extra_arm_ip = s_val[1];
            request_msg->delay_ms = d_val[0];

            auto response_msg = connect_carm_client->async_send_request(request_msg);

            if (rclcpp::spin_until_future_complete(ros_node, response_msg, 10s) != rclcpp::FutureReturnCode::SUCCESS)
            {
                mwarn("failed to call connect arm serverce!");
                return;
            }

            minfo("call connect arm service succeed!");
            auto &result = response_msg.get();
            minfo("service result: %d", result->ret);
        }
        else if (cmd_name == "setDebug")
        {
            if (!set_debug_client->wait_for_service(3s))
            {
                if (!rclcpp::ok())
                {
                    mwarn("服务等待中断");
                    return;
                }
                mwarn("服务等待超时");
                return;
            }
            auto request_msg = std::make_shared<test_msg::srv::SetDebug::Request>();

            request_msg->flag = b_val[0];

            auto response_msg = set_debug_client->async_send_request(request_msg);

            if (rclcpp::spin_until_future_complete(ros_node, response_msg, 10s) != rclcpp::FutureReturnCode::SUCCESS)
            {
                mwarn("failed to call setdebug serverce!");
                return;
            }

            minfo("call setdebug service succeed!");
            auto &result = response_msg.get();
            minfo("service result: %d", result->ret);
        }
        else if (cmd_name == "ready")
        {
            if (!set_ready_client->wait_for_service(3s))
            {
                if (!rclcpp::ok())
                {
                    mwarn("服务等待中断");
                    return;
                }
                mwarn("服务等待超时");
                return;
            }
            auto request_msg = std::make_shared<test_msg::srv::SetControllerReady::Request>();

            request_msg->feedback = true;
            request_msg->delay_ms = 100;

            auto response_msg = set_ready_client->async_send_request(request_msg);

            if (rclcpp::spin_until_future_complete(ros_node, response_msg, 10s) != rclcpp::FutureReturnCode::SUCCESS)
            {
                mwarn("failed to call setready serverce!");
                return;
            }

            minfo("call setready service succeed!");
            auto &result = response_msg.get();
            minfo("service result: %d", result->ret);
        }
        else if(cmd_name == "move_joint"){
            if (!move_joint_client->wait_for_service(3s))
            {
                if (!rclcpp::ok())
                {
                    mwarn("服务等待中断");
                    return;
                }
                mwarn("服务等待超时");
                return;
            }
            auto request_msg = std::make_shared<test_msg::srv::FastToJointPos::Request>();
            for(int i = 0; i < 6; i++){
                request_msg->target_joint_pos.push_back(arr_val[0][i]);
            }
            request_msg->speed = d_val[0];
            request_msg->priority = 0;
            request_msg->extra_request = "";

            auto response_msg = move_joint_client->async_send_request(request_msg);

            if (rclcpp::spin_until_future_complete(ros_node, response_msg, 10s) != rclcpp::FutureReturnCode::SUCCESS)
            {
                mwarn("failed to call movejoint serverce!");
                return;
            }

            minfo("call movejoint service succeed!");
            auto &result = response_msg.get();
            minfo("service result: %c%s", "cyan", result->ret.c_str());
        }
        else if(cmd_name == "transfer"){
            if (!transfer_file_client->wait_for_service(3s))
            {
                if (!rclcpp::ok())
                {
                    mwarn("服务等待中断");
                    return;
                }
                mwarn("服务等待超时");
                return;
            }
            auto request_msg = std::make_shared<test_msg::srv::TransferFile::Request>();

            request_msg->inpath = s_val[0];

            auto response_msg = transfer_file_client->async_send_request(request_msg);

            if (rclcpp::spin_until_future_complete(ros_node, response_msg, 10s) != rclcpp::FutureReturnCode::SUCCESS)
            {
                mwarn("failed to call movejoint serverce!");
                return;
            }

            minfo("call movejoint service succeed!");
            auto &result = response_msg.get();
            minfo("service result: %c%s", "cyan", result->ret.c_str());
        }
    
```

***
publish pointcloud
发布点云的类需要继承rclcpp::Node，将pcd文件的路径传入构造函数，并且构造发布点云的节点
将pcd文件的路径读到pcl::pclpointcloud2 cloud里，设置体素滤波器对点云作稀疏处理，将滤波后的的点云放入sensor_msgs::pointcloud2类型的pcl_output里
```c++
  pcl::PCLPointCloud2::Ptr cloud(new pcl::PCLPointCloud2());
        pcl::PCLPointCloud2 cloud_filtered;

        pcl::PCDReader reader;
        reader.read(pcd_file_path, *cloud); // 读取pcd文件，相对路径(相对于可执行文件的位置)

        // Create the filtering object
        // 设置体素滤波器
        pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
        sor.setInputCloud(cloud);             // 输入点云
        sor.setLeafSize(0.02f, 0.02f, 0.02f); // 体素滤波器，单位m
        sor.filter(cloud_filtered);           // 滤波后的点云
        pcl_conversions::fromPCL(cloud_filtered, pcl_output);
        pcl_output.header.frame_id = "map";
```
create_publish创建发布者，发布话题为pub_pointcloud

```c++
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/voxel_grid.h>


#include "rclcpp/rclcpp.hpp"

class PcdPublisher : public rclcpp::Node
{
public:
    PcdPublisher(const std::string &pcd_file_path) : rclcpp::Node("pcb_pub")
    {
        
        pcl::PCLPointCloud2::Ptr cloud(new pcl::PCLPointCloud2());
        pcl::PCLPointCloud2 cloud_filtered;

        pcl::PCDReader reader;
        reader.read(pcd_file_path, *cloud); // 读取pcd文件，相对路径(相对于可执行文件的位置)

        // Create the filtering object
        // 设置体素滤波器
        pcl::VoxelGrid<pcl::PCLPointCloud2> sor;
        sor.setInputCloud(cloud);             // 输入点云
        sor.setLeafSize(0.02f, 0.02f, 0.02f); // 体素滤波器，单位m
        sor.filter(cloud_filtered);           // 滤波后的点云
        pcl_conversions::fromPCL(cloud_filtered, pcl_output);
        pcl_output.header.frame_id = "map";

        pcl_pub = this->create_publisher<sensor_msgs::msg::PointCloud2>("pub_pointcloud", 10);
        pub_timer = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&PcdPublisher::timer_callback, this));
    }

private:
    void timer_callback()
    {
        pcl_pub->publish(pcl_output);
    }

private:
    rclcpp::TimerBase::SharedPtr pub_timer;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pcl_pub;
    sensor_msgs::msg::PointCloud2 pcl_output;
};
int main(int argc, char *argv[])
{
    std::string pcd_file_path;

    pcd_file_path = "/home/nixiaowei/Desktop/data/tutorials/room_scan1.pcd";
    
    
    rclcpp::init(argc, argv);
    rclcpp::Rate(std::chrono::seconds(1));
    rclcpp::spin(std::make_shared<PcdPublisher>(pcd_file_path));
    rclcpp::shutdown();
    return 0;
}
```
***
运行点云的发布者的可执行文件以后可以在rviz2上通过订阅查询到点云的状态

#summaray
1.定义一个键盘事件，在carm_controller通讯层增加remote接口及websocket map，并且在sdk上增加对应接口，实现carm_controller和sdk的通讯
2.实现将回传到sdk的消息通过自定义的msg发布
3.实现将文件通过srv消息拷贝到指定目录
4.自己创建一个package对外提供服务，自定义msg发布
5.ros_client
6.发布pointclient


***

#序列化与反序列化
利用msgpack做序列化与反序列化，在sdk与controller中传递消息
```c++
    std::string testmsgpack(string name_, int age_)
    {
        minfo("testmsgpack");
        Json::Value request_json;
        request_json["command"] = "testmsgpack";
        request_json["name"] = name_;
        request_json["age"] = age_;

        string res = com_global.webSendTasks(request_json);
        log_time::TimerUnit timeout_timer;
        log_time::timer_start(timeout_timer);
        while (1)
        {
            log_time::timer_end(timeout_timer);
            if (timeout_timer.duration_ms > 50000)
            {
                mwarn("通讯超时...");
                return "";
            }
            ResponseCache cache = com_global.getResponseCache();
            if (cache.is_cache && cache.name == "testmsgpack")
            {
                try
                {
                    std::stringstream sbuf;
                    msgpack::pack(sbuf, cache.detail);//将cache.detail里的内容序列化到sbuf内
                    
                    std::string const &str = sbuf.str();//将sbuf内的字符串赋到str内
                    print(str);//str内存储的是二进制流
                    minfo("recieve msg: %c%s", "blue", str.c_str());
                    msgpack::object_handle oh =
                    msgpack::unpack(str.data(), str.size());//反序列化
                    msgpack::object obj = oh.get();
                    std::cout << obj << std::endl;
                }
                catch (...)
                {
                    std::cout << "catch error" << std::endl;
                }
            }
        }
    }

```
