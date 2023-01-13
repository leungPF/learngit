#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/DNS.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerResponse.h"


class RootHandler: public Poco::Net::HTTPRequestHandler
{
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response)
    {
        //
        std::cout << request.getURI() << std::endl;
        std::cout << request.getVersion() << std::endl;
        std::cout << request.getContentType() << std::endl;
        std::cout << request.getTransferEncoding() << std::endl;

        std::string recv_string;
        Poco::StreamCopier::copyToString(request.stream(), recv_string);
        std::cout << recv_string << std::endl;
        //

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");
        std::ostream& ostr = response.send();
        ostr << "<html><head><title>HTTP Server powered by POCO C++ Libraries</title></head>";
        ostr << "<body>";
        ostr << "<h1>hello</h1>";
        ostr << "</body></html>";
    }
};

class MyRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory{
public:
    MyRequestHandlerFactory(){};
    ~MyRequestHandlerFactory(){};
public:
    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request)
   {
        return new RootHandler;
   };
};


int main(int argc, char** argv)
{

    Poco::UInt16 port = 1080;
    Poco::Net::HTTPServerParams * pParams = new Poco::Net::HTTPServerParams;
    pParams->setMaxQueued(10);
    pParams->setMaxThreads(1);
    Poco::Net::ServerSocket svs(port); // set-up a server socket
    Poco::Net::HTTPServer srv(new MyRequestHandlerFactory(), svs, pParams);
    // start the HTTPServer
    srv.start();
    //waitForTerminationRequest();
    getchar();
    // Stop the HTTPServer
    srv.stop();
    return 0;
}
