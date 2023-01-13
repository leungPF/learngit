#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>
#include <string>

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

std::string mystring;
class RootHandler: public Poco::Net::HTTPRequestHandler
{
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
        Poco::Net::HTTPServerResponse& response) noexcept
    {
        std::cout << request.getURI() << std::endl;
        std::cout << request.getVersion() << std::endl;
        std::cout << request.getContentType() << std::endl;
        std::cout << request.getTransferEncoding() << std::endl;

        
        std::string recv_string;
        Poco::StreamCopier::copyToString(request.stream(), recv_string);
        std::cout << recv_string << std::endl;
        

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");
        std::ostream& ostr = response.send();
        /// Sends the response header to the client and
		/// returns an output stream for sending the
		/// response body.
        ostr << "message send back to client,this message define in http_server"<<std::endl;
        ostr<<recv_string<<std::endl;
        std::cout<<"---------------"<<std::endl;
        //ostr << "<body>";
        //ostr << "<h1>hello</h1>";
        //ostr << "</body></html>";
        //ostr<<mystring;
    }
};

class MyRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory{
public:
    MyRequestHandlerFactory(){};
    ~MyRequestHandlerFactory(){};
public:
    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) noexcept
   {
        return new RootHandler;
   };
};


int main(int argc, char** argv)
{

    Poco::UInt16 port = 1080;
    Poco::Net::HTTPServerParams * pParams = new Poco::Net::HTTPServerParams;
    pParams->setMaxQueued(10);
    pParams->setMaxThreads(4);
    Poco::Net::ServerSocket svs(port); // set-up a server socket
    Poco::Net::HTTPServer srv(new MyRequestHandlerFactory(), svs, pParams);
    // start the HTTPServer
    srv.start();
    std::cout<<mystring<<std::endl;
    //waitForTerminationRequest();
    getchar();
    // Stop the HTTPServer
    srv.stop();
    return 0;
}
