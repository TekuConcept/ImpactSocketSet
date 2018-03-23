/**
 * Created by TekuConcept on July 27, 2017
 */

#ifndef RFC_WEBSOCKET_CLIENT_NODE_H
#define RFC_WEBSOCKET_CLIENT_NODE_H

#include "RFC/ResponseMessage.h"
#include "RFC/Websocket.h"
#include "RFC/URI.h"

namespace Impact {
namespace RFC6455 {
    class WebsocketClientNode : public RFCWebsocket {
    public:
        WebsocketClientNode(std::iostream &stream, URI uri);
        
        bool initiateHandshake();
        bool acceptHandshake();
    
    private:
        URI _uri_;

        std::string generateKey();
        bool responseHelper(RFC2616::ResponseMessage message);
    };
}}

#endif