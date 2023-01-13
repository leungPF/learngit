#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>


#include <msgpack.hpp>

struct my_base1 {
    int a;
    MSGPACK_DEFINE(a);
};
inline bool operator==(my_base1 const& lhs, my_base1 const& rhs) {
    return lhs.a == rhs.a;
}

struct my_base2 {
    std::string b;
    std::string c;
    MSGPACK_DEFINE(b, c);
};
inline bool operator==(my_base2 const& lhs, my_base2 const& rhs) {
    return lhs.b == rhs.b && lhs.c == rhs.c;
}

class my_class : public my_base1, private my_base2 {
public:
    my_class() {} // When you want to convert from msgpack::object to my_class,
                  // my_class should be default constractible.
    my_class(std::string const& name, int age):name_(name), age_(age) {}
    void set_b(std::string const& str) { b = str; }
    void set_c(std::string const& str) { c = str; }
    friend bool operator==(my_class const& lhs, my_class const& rhs) {
        return
            static_cast<my_base1 const&>(lhs) == static_cast<my_base1 const&>(rhs) &&
            static_cast<my_base2 const&>(lhs) == static_cast<my_base2 const&>(rhs) &&
            lhs.name_ == rhs.name_ && lhs.age_ == rhs.age_;
    }

private:
    std::string name_;
    int age_;

public:
    MSGPACK_DEFINE(name_, age_, MSGPACK_BASE(my_base1), MSGPACK_BASE(my_base2));
};

void print(std::string const& buf) {
    for (std::string::const_iterator it = buf.begin(), end = buf.end();
         it != end;
         ++it) {
        std::cout
            << std::setw(2)
            << std::hex
            << std::setfill('0')
            << (static_cast<int>(*it) & 0xff)
            << ' ';
    }
    std::cout << std::dec << std::endl;
}

int main() {
    {   // pack, unpack
        my_class my("John Smith", 42);
        my.a = 123;
        my.set_b("ABC");
        my.set_c("DEF");
        std::stringstream ss;
        msgpack::pack(ss, my);

        std::string const& str = ss.str();
        print(str);

        msgpack::object_handle oh =
            msgpack::unpack(str.data(), str.size());
        msgpack::object obj = oh.get();
        std::cout << obj << std::endl;
        assert(obj.as<my_class>() == my);
    }
    {   // create object with zone
        my_class my("John Smith", 42);
        my.a = 123;
        my.set_b("ABC");
        my.set_c("DEF");
        msgpack::zone z;
        msgpack::object obj(my, z);
        std::cout << obj << std::endl;
        assert(obj.as<my_class>() == my);
    }
}

// #include <vector>
// #include <string>
// #include <iostream>
// #include <msgpack.hpp>

// //定义命令
// #define COMMAND_GET_INFO 0x01

// //自定义请求类
// class MyClass {
// public:
//     std::string m_str;
//     std::vector<int> m_vec;
// public:
//     MSGPACK_DEFINE(m_str, m_vec);
// };

// int main() {
// 	//参数初始化
//     MyClass my_request;
//     my_request.m_str ="Hello MsgPack";
//     my_request.m_vec.push_back(100);

//     std::tuple<int, MyClass, std::string> send_msg(COMMAND_GET_INFO, my_request, "Addition Data");
	
// 	//序列化组包
//     msgpack::sbuffer sbuf;
//     msgpack::pack(sbuf, send_msg);
    
// 	//网络传送使用字符串作为消息
//     std::string str(sbuf.data());
//     std::cout<<str<<std::endl;
// 	//反序列化解包
//     msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());
//     msgpack::object deserialized_object = oh.get();
// 	//转换
//     std::tuple<int, MyClass, std::string> recv_msg;
//     deserialized_object.convert(recv_msg);
    
// 	//打印
//     std::cout << std::get<0>(recv_msg)<< std::endl;
//     std::cout << std::get<1>(recv_msg).m_str<< std::endl;
//     std::cout << std::get<2>(recv_msg)<< std::endl;
// }
