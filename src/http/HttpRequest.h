
typedef struct
{
    unsigned long len;
    unsigned char *data;
} ngx_str_t;

typedef struct
{
    unsigned long hash;
    ngx_str_t key;
    ngx_str_t value;
    unsigned char *lowcase_key;
} ngx_table_elt_t;

typedef struct
{
    ngx_list_t headers;

    ngx_table_elt_t *host;
    ngx_table_elt_t *connection;
    ngx_table_elt_t *if_modified_since;
    ngx_table_elt_t *if_unmodified_since;
    ngx_table_elt_t *if_match;
    ngx_table_elt_t *if_none_match;
    ngx_table_elt_t *user_agent;
    ngx_table_elt_t *referer;
    ngx_table_elt_t *content_length;
    ngx_table_elt_t *content_range;
    ngx_table_elt_t *content_type;

    ngx_table_elt_t *range;
    ngx_table_elt_t *if_range;

    ngx_table_elt_t *transfer_encoding;
    ngx_table_elt_t *te;
    ngx_table_elt_t *expect;
    ngx_table_elt_t *upgrade;

#if (NGX_HTTP_GZIP || NGX_HTTP_HEADERS)
    ngx_table_elt_t *accept_encoding;
    ngx_table_elt_t *via;
#endif

    ngx_table_elt_t *authorization;

    ngx_table_elt_t *keep_alive;

#if (NGX_HTTP_X_FORWARDED_FOR)
    ngx_array_t x_forwarded_for;
#endif

#if (NGX_HTTP_REALIP)
    ngx_table_elt_t *x_real_ip;
#endif

#if (NGX_HTTP_HEADERS)
    ngx_table_elt_t *accept;
    ngx_table_elt_t *accept_language;
#endif

#if (NGX_HTTP_DAV)
    ngx_table_elt_t *depth;
    ngx_table_elt_t *destination;
    ngx_table_elt_t *overwrite;
    ngx_table_elt_t *date;
#endif

    ngx_str_t user;
    ngx_str_t passwd;

    ngx_array_t cookies;

    ngx_str_t server;
    off_t content_length_n;
    time_t keep_alive_n;

    unsigned connection_type : 2;
    unsigned chunked : 1;
    unsigned msie : 1;
    unsigned msie6 : 1;
    unsigned opera : 1;
    unsigned gecko : 1;
    unsigned chrome : 1;
    unsigned safari : 1;
    unsigned konqueror : 1;
} ngx_http_headers_in_t;