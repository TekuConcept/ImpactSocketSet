/**
 * Created by TekuConcept on October 4, 2019
 */

#include <sstream>
#include <gtest/gtest.h>
#include "utils/environment.h"
#include "rfc/http/transfer_coding.h"

using namespace impact;
using namespace http;

#define NO_THROW_BEGIN try {
#define NO_THROW_END   } catch (...) { FAIL(); }
#define THROW_BEGIN   try {
#define THROW_END     FAIL(); } catch (...) { }


class good_transfer_coding : public transfer_coding {
public:
    good_transfer_coding()
    : transfer_coding("FooBar") { }
    ~good_transfer_coding() = default;
    std::string encode(const std::string& _) { return _; };
};


class bad_transfer_coding : public transfer_coding {
public:
    bad_transfer_coding()
    : transfer_coding("\x7F") { } // TCHAR only
    ~bad_transfer_coding() = default;
    std::string encode(const std::string& _) { return _; };
};


class reserved_transfer_coding : public transfer_coding {
public:
    reserved_transfer_coding()
    : transfer_coding("chunked") { } // chunked name reserved
    ~reserved_transfer_coding() = default;
    std::string encode(const std::string& _) { return _; };
};


TEST(test_http_transfer_coding, basic_coding)
{
    NO_THROW_BEGIN
        good_transfer_coding good;
    NO_THROW_END

    THROW_BEGIN
        bad_transfer_coding bad;
    THROW_END

    THROW_BEGIN
        reserved_transfer_coding bad;
    THROW_END
}


TEST(test_http_transfer_coding, chunked_extension)
{
    NO_THROW_BEGIN
        chunked_coding::extension_t extension("foo");
        EXPECT_EQ(extension.to_string(), ";foo");
    NO_THROW_END

    NO_THROW_BEGIN
        chunked_coding::extension_t extension("foo", "bar");
        EXPECT_EQ(extension.to_string(), ";foo=bar");
    NO_THROW_END

    NO_THROW_BEGIN
        chunked_coding::extension_t extension("foo", "\"bar\"");
        EXPECT_EQ(extension.to_string(), ";foo=\"bar\"");
    NO_THROW_END

    THROW_BEGIN
        chunked_coding::extension_t extension("foo", "\"ba\"r");
    THROW_END

    NO_THROW_BEGIN
        chunked_coding::extension_t extension("foo", "\"bar\"");
        EXPECT_EQ(extension.to_string(), ";foo=\"bar\"");
        extension.value("baz");
        EXPECT_EQ(extension.to_string(), ";foo=baz");
    NO_THROW_END

    THROW_BEGIN
        chunked_coding::extension_t extension("foo", "bar");
        extension.value("ba\"z");
    THROW_END

    NO_THROW_BEGIN
        chunked_coding::extension_t extension =
            chunked_coding::extension_t::parse(";foo=bar");
        EXPECT_EQ(extension.name(), "foo");
        EXPECT_EQ(extension.value(), "bar");
    NO_THROW_END

    THROW_BEGIN
        chunked_coding::extension_t extension =
            chunked_coding::extension_t::parse("foo=b\"ar");
    THROW_END
}


class dummy_chunked_observer : public chunked_observer {
public:
    ~dummy_chunked_observer() = default;

    void
    on_next_chunk(
        std::vector<chunked_coding::extension_t>& __extensions,
        const std::string&                        __buffer)
    {
        __extensions.push_back(chunked_coding::extension_t("foo"));
        __extensions.push_back(chunked_coding::extension_t("bar", "baz"));
        (void)__buffer;
    }

    void
    on_last_chunk(
        std::vector<chunked_coding::extension_t>& __extensions,
        header_list&                              __trailers)
    {
        __extensions.push_back(chunked_coding::extension_t("daisy"));
        __trailers.insert(__trailers.begin(), {
            // content-length is a forbidden trailer;
            // it will be discarded
            header_t(field_name::CONTENT_LENGTH, "42"),
            header_t("Upload-Status", "OK")
        });
    }
};


TEST(test_http_transfer_coding, chunked_coding)
{
    NO_THROW_BEGIN
        chunked_coding coding;
    NO_THROW_END

    {
        chunked_coding coding;
        dummy_chunked_observer observer;

        // [-- vanilla --]

        // basic message
        auto chunk = coding.encode("Hello World!");
        EXPECT_EQ(chunk, "C\r\nHello World!\r\n");

        // last message
        chunk = coding.encode("");
        EXPECT_EQ(chunk, "0\r\n\r\n");

        // [-- extended --]

        coding.register_observer(&observer);

        // basic message
        chunk = coding.encode("Hello World!");
        EXPECT_EQ(chunk, "C;foo;bar=baz\r\nHello World!\r\n");

        // last message
        chunk = coding.encode("");
        EXPECT_EQ(chunk,
            "0;daisy\r\n"
            "Upload-Status: OK\r\n"
            "\r\n");
    }
}