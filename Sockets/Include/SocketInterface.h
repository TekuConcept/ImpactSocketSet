/**
 * Created by TekuConcept on June 15, 2018
 */

#ifndef _SOCKET_INTERFACE_H_
#define _SOCKET_INTERFACE_H_

#include <iostream>
#include <string>            // For string
#include <cstring>           // For strerror, atoi, and memset
#include <exception>         // For exception class
#include <vector>

#include "SocketHandle.h"
#include "SocketPollTable.h"

#if defined(_MSC_VER)
	#include <winsock2.h>
#else
	#include <sys/poll.h>    // For struct pollfd, poll()
	#include <netinet/in.h>  // For sockaddr_in
#endif

namespace Impact {
	typedef struct KeepAliveOptions {
			int enabled;  /* Enables KEEPALIVE on the target socket connection.  */
			int idleTime; /* Number of idle seconds before sending a KA probe.   */
			int interval; /* How often in seconds to resend an unacked KA probe. */
			int retries;  /* How many times to resend a KA probe if previous
			                 probe was unacked.                                  */
			KeepAliveOptions();
	} KeepAliveOptions;

	
	class SocketInterface {
		SocketInterface(); // static-only class

		static std::string getHostErrorMessage();
		static void fillAddress(const std::string&, unsigned short port,
			sockaddr_in&);

	public:
		/*                    *\
		| GENERIC SOCKET INFO  |
		\*                    */
		static std::string getErrorMessage();
		static std::string getLocalAddress(const SocketHandle& handle)
			/* throw(std::runtime_error) */;
		static unsigned short getLocalPort(const SocketHandle& handle)
			/* throw(std::runtime_error) */;
		static void setLocalPort(const SocketHandle& handle, unsigned short localPort)
			/* throw(std::runtime_error) */;
		static void setLocalAddressAndPort(const SocketHandle& handle,
			const std::string& localAddress, unsigned short localPort = 0)
			/* throw(std::runtime_error) */;
		static unsigned short resolveService(const std::string& service,
			const std::string& protocol = "tcp");
		static std::string getForeignAddress(const SocketHandle& handle)
			/* throw(std::runtime_error) */;
		static unsigned short getForeignPort(const SocketHandle& handle)
			/* throw(std::runtime_error) */;

		/*                    *\
		| COMMUNICATION        |
		\*                    */
		static void connect(const SocketHandle& handle,
			const std::string& foreignAddress, unsigned short foreignPort)
			/* throw(std::runtime_error) */;
		static void shutdown(const SocketHandle& handle,
			SocketChannel channel = SocketChannel::BOTH)
			/* throw(std::runtime_error) */;
		static void send(const SocketHandle& handle, const void* buffer, int bufferLen,
			MessageFlags flags = MessageFlags::NONE)
			/* throw(std::runtime_error) */;
		static int recv(const SocketHandle& handle, void* buffer, int bufferLen,
			MessageFlags flags = MessageFlags::NONE)
			/* throw(std::runtime_error) */;
		static void keepalive(const SocketHandle& handle,
			KeepAliveOptions options)
			/* throw(std::runtime_error) */;
		static int select(
			std::vector<SocketHandle*> readHandles,
			std::vector<SocketHandle*> writeHandles,
			int timeout=-1, unsigned int microTimeout=0)
			/* throw(std::runtime_error) */;
		// poll
	};
}

#endif