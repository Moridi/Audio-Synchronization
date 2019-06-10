#ifndef SERVER_H_
#define SERVER_H_

void setup_client(int argc, char *argv[], int client_id);
void initialize_pipeline(int argc, char *argv[]);
void add_audio_elements(int client_id);
void add_rtpbin();
void add_udp_terminals(int rtp_sink_port,
    int rtcp_sink_port, int rtcp_src_port, int client_id);
void link_to_rtpbin(const char* request_pad, int client_id);
void get_rtp_source_pad(const char* static_pad, int client_id);
void get_rtcp_source_pad(const char* request_pad, int client_id);
void receive_rtcp(const char* request_pad, int client_id);
void setup_pipeline();

#endif