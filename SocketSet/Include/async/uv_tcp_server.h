/**
 * Created by TekuConcept on January 19, 2021
 */

#ifndef IMPACT_UV_TCP_SERVER_H
#define IMPACT_UV_TCP_SERVER_H

#include <memory>
#include <atomic>
#include <vector>
#include <future>
#include "uv.h"
#include "interfaces/tcp_server_interface.h"
#include "interfaces/uv_node_interface.h"
#include "async/uv_event_loop.h"

namespace impact {

    class uv_tcp_server :
        public tcp_server_interface,
        public uv_child_interface,
        protected tcp_server_observer_interface
    {
    public:
        uv_tcp_server(uv_event_loop* event_loop);
        ~uv_tcp_server();

        tcp_address_t address() const override;
        bool listening() const override;
        size_t max_connections() const override;
        void max_connections(size_t value) override;

        void close(event_emitter::callback_t cb = nullptr) override;
        void listen(
            std::string path,
            event_emitter::callback_t cb = nullptr) override;
        void listen(
            unsigned short port = 0,
            std::string host = "127.0.0.1",
            event_emitter::callback_t cb = nullptr) override;

        void set_event_observer(tcp_server_observer_interface*) override;

        void send_signal(uv_node_signal_t op) override;

    protected:
        void on_close() override;
        void on_connection(const tcp_client_t& connection) override;
        void on_error(const std::string& message) override;
        void on_listening() override;

    private:
        uv_event_loop*                 m_event_loop;
        tcp_server_observer_interface* m_fast_events;
        std::atomic<bool>              m_is_listening;
        size_t                         m_max_connections;
        tcp_address_t                  m_address;
        uv_tcp_t                       m_handle;
        struct addrinfo                m_hints;
        std::atomic<size_t>            m_client_count;

        void _M_close();
        void _M_listen(unsigned short port, std::string host);
        void _M_listen(std::string path);
        void _M_listen(const struct sockaddr*);
        void _M_emit_listen_error(int status);

        friend class uv_tcp_client;
        friend void uv_tcp_server_on_close(uv_handle_t*);
        friend void uv_tcp_server_on_connection(uv_stream_t*, int);
        friend void uv_tcp_server_on_path_resolved(
            uv_getaddrinfo_t*, int, struct addrinfo*);
    };

} /* namespace impact */

#endif /* IMPACT_UV_TCP_SERVER_H */
