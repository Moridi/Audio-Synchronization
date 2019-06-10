#ifndef SERVER_H_
#define SERVER_H_

void setup_client(int argc, char *argv[], int client_id);
void initialize_pipeline(int argc, char *argv[]);
void add_rtpbin();
void add_udp_terminals(int rtp_sink_port,
    int rtcp_sink_port, int rtcp_src_port);
void link_to_rtpbin(const char* request_pad);
void get_rtp_source_pad(const char* static_pad);
void get_rtcp_source_pad(const char* request_pad);
void receive_rtcp(const char* request_pad);
void setup_pipeline();

#endif