/**
 * Created by TekuConcept on March 7, 2021
 */

#ifndef IMPACT_INTERFACES_SECURE_CLIENT_INTERFACE_H
#define IMPACT_INTERFACES_SECURE_CLIENT_INTERFACE_H

#include <string>
#include <memory>
#include "interfaces/tcp_client_interface.h"
#include "interfaces/secure_x509_certificate_interface.h"

namespace impact {

    class secure_client_interface :
        public tcp_client_interface,
        public secure_x509_certificate_interface
    {
    public:
        virtual ~secure_client_interface() = default;

        virtual const std::string& server_name() const = 0;
        virtual void server_name(std::string host) = 0;
        virtual bool cert_verify_enabled() const = 0;
        virtual void cert_verify_enabled(bool enabled) = 0;
    };
    typedef std::shared_ptr<secure_client_interface> secure_client_t;

} /* namespace impact */

#endif /* IMPACT_INTERFACES_SECURE_CLIENT_INTERFACE_H */