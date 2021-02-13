/**
 * Created by TekuConcept on February 11, 2021
 */

#ifndef IMPACT_UV_TCP_CLIENT_H
#define IMPACT_UV_TCP_CLIENT_H

#include <memory>
#include <atomic>
#include "uv.h"
#include "sockets/tcp_client_interface.h"
#include "async/uv_event_loop.h"

namespace impact {

    class uv_tcp_client : public tcp_client_interface {
    public:
        uv_tcp_client(uv_event_loop* event_loop);
        ~uv_tcp_client();

        tcp_address_t address() const override;
        size_t bytes_read() const override;
        size_t bytes_written() const override;
        bool connecting() const override;
        bool destroyed() const override;
        std::string local_address() const override;
        unsigned short local_port() const override;
        bool pending() const override;
        std::string remote_address() const override;
        address_family remote_family() const override;
        unsigned short remote_port() const override;
        size_t timeout() const override;
        std::string ready_state() const override;

        tcp_client_interface* connect(
            std::string path,
            event_emitter::callback_t cb = nullptr) override;
        tcp_client_interface* connect(
            unsigned short port,
            event_emitter::callback_t cb) override;
        tcp_client_interface* connect(
            unsigned short port,
            std::string host = "127.0.0.1",
            event_emitter::callback_t cb = nullptr) override;
        tcp_client_interface* destroy(std::string error = std::string()) override;
        tcp_client_interface* end(event_emitter::callback_t cb) override;
        tcp_client_interface* end(
            std::string data,
            event_emitter::callback_t cb) override;
        tcp_client_interface* end(
            std::string data = std::string(),
            std::string encoding = "utf8",
            event_emitter::callback_t cb = nullptr) override;
        tcp_client_interface* pause() override;
        tcp_client_interface* resume() override;
        tcp_client_interface* set_encoding(
            std::string encoding = "utf8") override;
        tcp_client_interface* set_keep_alive(
            unsigned int initial_delay) override;
        tcp_client_interface* set_keep_alive(
            bool enable = false,
            unsigned int initial_delay = 0) override;
        tcp_client_interface* set_no_delay(
            bool no_delay = true) override;
        tcp_client_interface* set_timeout(
            unsigned int timeout,
            event_emitter::callback_t cb = nullptr) override;
        bool write(
            std::string data,
            event_emitter::callback_t cb = nullptr) override;
        bool write(
            std::string data,
            std::string encoding = "utf8",
            event_emitter::callback_t cb = nullptr) override;

        // * on('close', (hadError: bool) => void)
        // * on('connect', () => void)
        // * on('data', (data: string) => void)
        // on('drain', () => void)
        // * on('end', () => void)
        // * on('error', (error: Error) => void)
        // on('lookup', (err?: Error, address: string, family: address_family, host: string) => void)
        // * on('ready', () => void)
        // on('timeout', () => void)

    private:
        std::shared_ptr<struct uv_event_loop::context_t> m_elctx;
        uv_event_loop*  m_event_loop;
        tcp_address_t   m_address;
        tcp_address_t   m_local_address;
        tcp_address_t   m_remote_address;
        struct addrinfo m_hints;
        uv_tcp_t        m_handle;
        uv_connect_t    m_connect_handle;
        size_t          m_bytes_read;
        size_t          m_bytes_written;
        size_t          m_timeout;
        bool            m_has_timeout;
        std::string     m_encoding;
        etimer_id_t     m_timeout_handle;

        enum class ready_state_t {
            PENDING,
            OPENING,
            OPEN,
            READ_ONLY,
            WRITE_ONLY,
            DESTROYED
        };
        std::atomic<ready_state_t> m_ready_state;

        enum class request_type {
            NONE,
            SET_KEEPALIVE,
            SET_NO_DELAY,
            SET_TIMEOUT,
            CONNECT,
            CONNECT2,
            PAUSE,
            RESUME,
            WRITE,
            END,
            DESTROY
        };
        struct async_request_t {
            request_type type;
            union {
                bool keepalive_enable;
                bool no_delay_enable;
            };
            union {
                unsigned int keepalive_delay;
                unsigned int timeout;
                unsigned short connect_port;
            };
            std::string connect_host;
            std::string data;
            std::string encoding;
            event_emitter::callback_t cb;
            std::promise<void>* promise;
            async_request_t();
        };
        uv_rwlock_t                         m_lock;
        uv_async_t                          m_async_handle;
        std::vector<struct async_request_t> m_requests;

        struct write_context_t {
            uv_tcp_client* client;
            std::string data;
            event_emitter::callback_t cb;
        };

        uv_tcp_client() = default;

        void _M_set_keepalive(bool, unsigned int);
        void _M_set_keepalive_async(bool, unsigned int);
        void _M_set_no_delay(bool);
        void _M_set_no_delay_async(bool);
        void _M_connect(std::string);
        void _M_connect_async(std::string);
        void _M_connect(unsigned short, std::string);
        void _M_connect_async(unsigned short, std::string);
        void _M_connect(const struct sockaddr*);
        void _M_pause();
        void _M_pause_async();
        void _M_resume();
        void _M_resume_async();
        void _M_write(std::string, std::string, event_emitter::callback_t);
        void _M_write_async(std::string, std::string, event_emitter::callback_t);
        void _M_set_timeout(unsigned int);
        void _M_set_timeout_async(unsigned int);
        void _M_end(std::string, std::string);
        void _M_end_async(std::string, std::string);
        void _M_destroy(std::string);
        void _M_destroy_async(std::string, bool block = false);
        void _M_emit_error_code(std::string, int);
        void _M_update_addresses();
        void _M_fill_address_info(const struct sockaddr*, tcp_address_t*);

        friend class uv_tcp_server;
        friend void uv_tcp_client_on_path_resolved(
            uv_getaddrinfo_t*, int, struct addrinfo*);
        friend void uv_tcp_client_on_connect(uv_connect_t*, int);
        friend void uv_tcp_client_on_data(uv_stream_t*, ssize_t, const uv_buf_t*);
        friend void uv_tcp_client_on_write(uv_write_t*, int);
        friend void uv_tcp_client_on_shutdown(uv_shutdown_t*, int);
        friend void uv_tcp_client_on_close(uv_handle_t* __handle);
        friend void uv_tcp_client_async_callback(uv_async_t*);
    };

} /* namespace impact */

#endif /* IMPACT_UV_TCP_CLIENT_H */
