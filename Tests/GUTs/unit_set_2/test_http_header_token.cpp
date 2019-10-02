/**
 * Created by TekuConcept on December 29, 2018
 */

#include <gtest/gtest.h>
#include "utils/environment.h"
#include "rfc/http/header_token.h"

using namespace impact;
using namespace http;

#define NO_THROW(code) try { {code} } catch (...) { FAIL(); }
#define THROW(code)    try { {code} FAIL(); } catch (...) { }

TEST(test_http_header_token, header_token)
{
    NO_THROW(
        header_token token("name", "value");
        EXPECT_EQ(token.name(), "Name"); /* case insensitive */
        EXPECT_EQ(token.value(), "value");
    )
    NO_THROW( // field value whitespace trimming
        header_token token("name", "\tset value ");
        EXPECT_EQ(token.value(), "set value");
    )
    NO_THROW( // skip field name validation check
        header_token token(field_name::CONNECTION, "value");
        EXPECT_EQ(token.name(), "Connection");
        EXPECT_EQ(token.value(), "value");
    )
    THROW(header_token token("/bad", "value");) // TCHAR only
    THROW(header_token token("name", "\x7F");)  // VCHAR only
    
    NO_THROW(header_token token("name: value\r\n");)
    // obsolete line folding
    NO_THROW(
        header_token token("name: value,\r\n\tvalue2\r\n");
        EXPECT_EQ(token.name(), "name");
        EXPECT_EQ(token.value(), "value, value2");
    )
    THROW(header_token token("name : bad\r\n");)
    THROW(header_token token("name: bad,\r\nvalue\r\n");)
}
