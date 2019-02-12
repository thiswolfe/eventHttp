#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/http.h>
#include <evhttp.h>

using namespace std;

void http_request_done(struct evhttp_request *req, void *arg)
{
  printf("send request ok...\n");
  size_t len = evbuffer_get_length(req->input_buffer);
  unsigned char * str = evbuffer_pullup(req->input_buffer, len);
  char buf[256] = { 0 };
  memcpy(buf, str, len);
  if(str == NULL)
  {
    printf("len = %d, str == NULL\n", len);
  }
  else
  {
    printf("len = %d, str = %s\n", len, buf);
  }
  event_base_loopbreak((struct event_base*)arg);
}
 
int main()
{
    struct event_base* base;
    struct evhttp_connection* conn;
    struct evhttp_request* req;
    
    base = event_base_new();
    //conn = evhttp_connection_new(base, NULL, "192.168.1.109", 8081);
    conn = evhttp_connection_new("127.0.0.1", 8081);
    evhttp_connection_set_base(conn, base);
 
    req = evhttp_request_new(http_request_done, base);
    evhttp_add_header(req->output_headers, "Host", "localhost");
    string get_data = "/get?sign=3.1415926&data=0.618";
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, get_data.c_str());
    evhttp_connection_set_timeout(req->evcon, 600);
    event_base_dispatch(base);

	// POST 请求
    req = evhttp_request_new(http_request_done, base);
    evhttp_add_header(req->output_headers, "Host", "localhost");
    string post_data = "{\"sign\":\"1+1=2\",\"data\":\"2+2=4\"}";
    evbuffer_add(req->output_buffer, post_data.c_str(), post_data.length());
    evhttp_make_request(conn, req, EVHTTP_REQ_POST, "/post");
    event_base_dispatch(base);

    evhttp_connection_free(conn);
    event_base_free(base);
    printf("run over...\n");
    return 0;
}
