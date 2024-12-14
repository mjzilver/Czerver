#include "http_codes.h"   

const char *ok_200 = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
const char *not_found_404 = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n";
const char *internal_server_error_500 = "HTTP/1.1 500 Internal Server Error\nContent-Type: text/plain\n\n";
