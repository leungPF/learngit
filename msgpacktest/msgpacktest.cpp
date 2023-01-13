#include<msgpack.hpp>
#include<tuple>
#include<iostream>
#include<string>

class Myclass
{
    public:
    std::string m_str;
    std::vector<int> m_vec;
    MSGPACK_DEFINE(m_str,m_vec);

};


int main(void)
{
    Myclass myclass;
    myclass.m_str="hello world";
    myclass.m_vec.push_back(100);

    std::tuple<int,Myclass,std::string> send_msg(1,myclass,"addition Date");

    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf,send_msg);

    std::string str(sbuf.data());
    std::cout<<str<<std::endl;

    msgpack::object_handle oh=msgpack::unpack(str.data(),str.size());

    std::tuple<int,Myclass,std::string> recv_msg;
    msgpack::object deserialized_object=oh.get();
  deserialized_object.convert(recv_msg);
    
	
    std::cout << std::get<0>(recv_msg)<< std::endl;
    std::cout << std::get<1>(recv_msg).m_str<< std::endl;
    std::cout << std::get<2>(recv_msg)<< std::endl;
}