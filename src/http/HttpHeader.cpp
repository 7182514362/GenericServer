#include "HttpHeader.h"
using namespace generic::http;

#include <vector>
std::vector<std::pair<char *, char *>> fields = {
    // general fields
    {"Cache-Control", ""},
    {"Connection", ""},
    {"Date", ""},
    {"Pragma", ""},
    {"Trailer", ""},
    {"Transfer-Encoding", ""},
    {"Upgrade", "upgrade to other protocol"},
    {"Via", "proxy information"},
    {"Warning", "error message"},
    // request fields
    {"Accept", ""},
    {"Accept-Charset", ""},
    {"Accept-Encoding", ""},
    {"Accept-Language", ""},
    {"Authorization", ""},
    {"Expect", ""},
    {"From", ""},
    {"Host", ""},
    {"If-Match", ""},
    {"If-Modified-Since", ""},
    {"If-None-Match", ""},
    {"If-Range", ""},
    {"If-Unmodified-Since", ""},
    {"Max-Forwards", ""},
    {"Proxy-Authorization", ""},
    {"Range", ""},
    {"Referer", ""},
    {"TE", ""},
    {"User-Agent", ""},
    // responde fields
    {"Accept-Ranges", ""},
    {"Age", ""},
    {"ETag", ""},
    {"Location", ""},
    {"Proxy-Authenticate", ""},
    {"Retry-After", ""},
    {"Server", ""},
    {"Vary", ""},
    {"WWW-Authenticate", ""},
    // entity fields
    {"Allow", ""},
    {"Content-Encoding", ""},
    {"Content-Language", ""},
    {"Content-Length", ""},
    {"Content-Location", ""},
    {"Content-MD5", ""},
    {"Content-Range", ""},
    {"Content-Type", ""},
    {"Expires", ""},
    {"Last-Modified", ""},
    // others
    {"Cookie", ""},
    {"Set-Cookie", ""},
    {"Keep-Alive", ""},
    {"", ""}

};