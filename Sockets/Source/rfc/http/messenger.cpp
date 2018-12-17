/**
 * Created by TekuConcept on December 15, 2018
 */

#include "rfc/http/messenger.h"

#include <regex>
#include <iomanip>
#include "utils/environment.h"
#include "utils/errno.h"
// #include "rfc/http/types.h"

namespace impact {
namespace http {
namespace internal {
    const std::regex k_start_line_regex(
        "(([!#$%&'*+\\-.^_`|~0-9a-zA-Z]+) ([a-zA-Z0-9+\\-.:\\/_~%!$&'\\(\\)*,;="
        "@\\[\\]?]+|\\*) (HTTP\\/[0-9]\\.[0-9])\\r\\n)|((HTTP\\/[0-9]\\.[0-9]) "
        "([0-9][0-9][0-9]) ([\\t \\x21-\\x7E\\x80-\\xFF]*)\\r\\n)");
    const std::regex k_header_field_regex(
        "([!#$%&'*+\\-.^_`|~0-9a-zA-Z]+):[ \\t]*((?:[\\x21-\\x7E]+)?(?:[ \\t]+["
        "!-~]+)*)[ \\t]*\\r\\n");
}}}

using namespace impact;

/*  Note when parsind header fields:
    - preprend SP to field-value before splitting into tokens with regex
    - replace detected obs-fold with SP
    - determine obs-fold line matches field-value semantics before appending
*/

http::start_line::start_line()
: type(message_type::REQUEST),
  method("GET"), target("/"),
  http_major(1), http_minor(1)
{}


http::start_line::~start_line()
{}


http::start_line::start_line(const start_line& __rhs)
{
    this->type       = __rhs.type;
    this->http_major = __rhs.http_major;
    this->http_minor = __rhs.http_minor;
    if (this->type == message_type::REQUEST) {
        this->method = __rhs.method;
        this->target = __rhs.target;
    }
    else {
        this->message = __rhs.message;
        this->status  = __rhs.status;
    }
}


int
http::internal::parse_start_line(
    const std::string& __input,
    struct start_line* __result)
{
    /* match indicies
        0 - full string match
        1 - request match
        2 - request method
        3 - request target
        4 - request HTTP version
        5 - response match
        6 - response HTTP version
        7 - response status code
        8 - response status message
    */
    
    if (__result == NULL) {
        imp_errno = imperr::INVALID_ARGUMENT;
        return -1;
    }
    else imp_errno = imperr::SUCCESS;
    
    std::smatch match;
    if (std::regex_match(__input, match, k_start_line_regex)) {
        // current assumptions are that given 9 matches,
        // - four of the matches are blank
        // - three subsequent match indicies following 1 or 5 are not blank
        if (match.size() != 9) {
            imp_errno = imperr::HTTP_BAD_MATCH;
            return -1;
        }
        
        // typeof(match[i]) = std::ssub_match
        std::string http_version;
        if (match[1].str().size() != 0) {
            http_version      = match[4].str();
            __result->method  = match[2].str();
            __result->target  = match[3].str();
            __result->type    = message_type::REQUEST;
        }
        else if (match[5].str().size() != 0) {
            http_version      = match[6].str();
            __result->status  = std::stoi(match[7].str());
            __result->message = match[8].str();
            __result->type    = message_type::RESPONSE;
        }
        else {
            imp_errno = imperr::HTTP_BAD_MATCH;
            return -1;
        }
        
        __result->http_major  = http_version[5] - '0';
        __result->http_minor  = http_version[7] - '0';
    }
    else {
        imp_errno = imperr::HTTP_BAD_MATCH;
        return -1;
    }
    
    return 0;
}


int
http::internal::parse_header_line(
    const std::string&                  __input,
    std::pair<std::string,std::string>* __result)
{
    /* match indicies
        0 - full string match
        1 - field name
        2 - field value (trimmed)
    */
    
    if (__result == NULL) {
        imp_errno = imperr::INVALID_ARGUMENT;
        return -1;
    }
    else imp_errno = imperr::SUCCESS;
    
    std::smatch match;
    if (std::regex_match(__input, match, k_header_field_regex)) {
        if (match.size() != 3) {
            imp_errno = imperr::HTTP_BAD_MATCH;
            return -1;
        }
        // typeof(match[i]) = std::ssub_match
        __result->first  = match[1].str();
        __result->second = match[2].str();
    }
    else {
        imp_errno = imperr::HTTP_BAD_MATCH;
        return -1;
    }
    
    return 0;
}


http::message::message()
{}


http::message::~message()
{}


http::message_type
http::message::type() const noexcept
{
    return __start_line.type;
}


int
http::message::http_major() const noexcept
{
    return __start_line.http_major;
}


int
http::message::http_minor() const noexcept
{
    return __start_line.http_minor;
}


void
http::message::send(std::ostream* __stream) const
{
    if (__stream == NULL) return;
    auto& out = *__stream;

    if (this->__start_line.type == message_type::REQUEST) {
        out << this->__start_line.method << " ";
        out << this->__start_line.target << " ";
        out << "HTTP/";
        out << (int)this->__start_line.http_major << ".";
        out << (int)this->__start_line.http_minor << "\r\n";
    }
    else {
        out << "HTTP/";
        out << (int)this->__start_line.http_major << ".";
        out << (int)this->__start_line.http_minor << " ";
        out << std::setfill('0') << std::setw(3);
        out << this->__start_line.status << std::setfill(' ') << " ";
        out << this->__start_line.message << "\r\n";
    }
    
    // out << headers
    
    out << "\r\n";
    
    // out << body
}


http::request_message::request_message(
    std::string __method,
    std::string __target)
{
    // validate method: escape invalid characters, toupper
    // validate target: escape invalid characters

    this->__start_line.type       = message_type::REQUEST;
    this->__start_line.http_major = 1;
    this->__start_line.http_minor = 1;
    this->__start_line.method     = __method;
    this->__start_line.target     = __target;
}


http::request_message::~request_message()
{}


std::string
http::request_message::method() const noexcept
{
    return __start_line.method;
}


std::string
http::request_message::target() const noexcept
{
    return __start_line.target;
}
