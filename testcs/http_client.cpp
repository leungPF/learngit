#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/Net/HTTPCredentials.h>
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/Net/HTMLForm.h"
#include <iostream>
#include <string>


bool doRequest(Poco::Net::HTTPClientSession &session, Poco::Net::HTTPRequest &request, Poco::Net::HTTPResponse &response) noexcept
{
    session.sendRequest(request);
    std::istream &rs = session.receiveResponse(response);
    std::cout << response.getStatus() << " " << response.getReason() << std::endl;
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        Poco::StreamCopier::copyStream(rs, std::cout);
        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(rs, null);
        return false;
    }
}

int main(int argc, char **argv)
{
    try
    {
        // json
        // std::string body("hello this is client");
        std::cout << "input your string in client" << std::endl;
        std::string mystring;
        std::cin >> mystring;
        std::string body(mystring);

        // uri

        Poco::URI uri("http://127.0.0.1:1080/");
        // session
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        session.setKeepAlive(true);

        // request
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.getPathAndQuery(), Poco::Net::HTTPRequest::HTTP_1_1);
        // request.setContentType("application/json");
        request.add("Accept", "Agent-007");
        request.add("User-Agent", "xxxxoooo");
        request.setContentLength(body.length());
        //
        session.sendRequest(request) << body; // send the message to server

        // response
        Poco::Net::HTTPResponse res;
        std::string recv_string;
        std::istream &is = session.receiveResponse(res);
        // Receives the header for the response to the previous
        /// HTTP request.
        ///
        /// The returned input stream can be used to read
        /// the response body. The stream is valid until
        /// sendRequest() is called or the session is
        /// destroyed.
        int result = (int)res.getStatus();
        std::cout << "result:" << result << ", reason:" << res.getReason() << std::endl;

        Poco::StreamCopier::copyToString(is, recv_string);
        std::cout << "recv : " << std::endl
                  << recv_string << std::endl;
	std::string recv_body = res.get("Date");
        std::cout << "body : " << std::endl
                  << recv_body << std::endl;
    }
    catch (Poco::Exception &exc)
    {
        std::cerr << "catch an exception: " << exc.displayText() << std::endl;
        // throw 1;
        return 1;
    }
    catch (...)
    {
    }
    return 0;
}
