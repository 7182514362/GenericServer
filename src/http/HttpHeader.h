#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H
#include <string>
namespace generic
{
    namespace http
    {
        struct Fields
        {
            char *name;
            
            char *desc;
        };

        struct HttpHeaderFields
        {
            // general fields

            // request fields
            // response fields
            // entity fields
        };

        class HttpRequestHeader
        {
            std::string m_method;
            std::string m_uri;
            std::string m_version;
            std::string m_response_fields;
        };

        class HttpResponseHeader
        {

            std::string m_version;
            std::string m_code;
            std::string m_reason;

            std::string m_response_fields;
        };
    }
}

#endif