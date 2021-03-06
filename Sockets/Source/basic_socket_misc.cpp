/**
 * Created by TekuConcept on July 11, 2018
 */

#include "sockets/basic_socket.h"
#include "basic_socket_common.inc"

using namespace impact;

std::string
basic_socket::local_address()
{
    ASSERT_MOVED
    struct sockaddr_in address;
    auto address_length = sizeof(address);

    auto status = ::getsockname(
        m_info_->descriptor,
        (struct sockaddr*)&address,
        (socklen_t*)&address_length
    );

    ASSERT(status != SOCKET_ERROR)
    return inet_ntoa(address.sin_addr);
}


unsigned short
basic_socket::local_port()
{
    ASSERT_MOVED
    sockaddr_in address;
    auto address_length = sizeof(address);

    auto status = ::getsockname(
        m_info_->descriptor,
        (struct sockaddr*)&address,
        (socklen_t*)&address_length
    );

    ASSERT(status != SOCKET_ERROR)
    return ntohs(address.sin_port);
}


std::string
basic_socket::peer_address()
{
    ASSERT_MOVED
    struct sockaddr_in address;
    unsigned int address_length = sizeof(address);

    auto status = ::getpeername(
        m_info_->descriptor,
        (struct sockaddr*)&address,
        (socklen_t*)&address_length
    );

    ASSERT(status != SOCKET_ERROR)
    return inet_ntoa(address.sin_addr);
}


unsigned short
basic_socket::peer_port()
{
    ASSERT_MOVED
    struct sockaddr_in address;
    unsigned int address_length = sizeof(address);

    auto status = ::getpeername(
        m_info_->descriptor,
        (struct sockaddr*)&address,
        (socklen_t*)&address_length
    );

    ASSERT(status != SOCKET_ERROR)
    return ntohs(address.sin_port);
}


void
basic_socket::broadcast(bool __enabled)
{
    ASSERT_MOVED
    auto permission = __enabled ? 1 : 0;

    auto status = ::setsockopt(
        m_info_->descriptor,
        SOL_SOCKET,
        SO_BROADCAST,
        (CCHAR_PTR)&permission,
        sizeof(permission)
    );

    ASSERT(status != SOCKET_ERROR)
}


void
basic_socket::multicast_ttl(unsigned char __ttl)
{
    ASSERT_MOVED

    auto status = ::setsockopt(
        m_info_->descriptor,
        IPPROTO_IP,
        IP_MULTICAST_TTL,
        (CCHAR_PTR)&__ttl,
        sizeof(__ttl)
    );

    ASSERT(status != SOCKET_ERROR)
}


void
basic_socket::reuse_address(bool __enabled)
{
    ASSERT_MOVED
    int reuse = __enabled ? 1 : 0;
    auto option = SO_REUSEADDR;
#ifdef IMPACT_WIN_SECURE_REUSE
    option = SO_EXCLUSIVEADDRUSE;
#endif
    auto status = ::setsockopt(
        m_info_->descriptor,
        SOL_SOCKET,
        option,
        (const char*)&reuse,
        sizeof(reuse)
    );
    ASSERT(status != SOCKET_ERROR)
#ifdef SO_REUSEPORT
    status = ::setsockopt(
        m_info_->descriptor,
        SOL_SOCKET,
        SO_REUSEPORT,
        (const char*)&reuse,
        sizeof(reuse)
    );
    ASSERT(status != SOCKET_ERROR)
#endif
}
