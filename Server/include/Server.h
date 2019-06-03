#ifndef SERVER_H_
#define SERVER_H_

void initialize_pipeline(int argc, char *argv[]);
void add_rtpbin();
void add_udp_terminals();
void link_to_rtpbin();
void get_rtp_source_pad();
void get_rtcp_source_pad();
void receive_rtcp();
void setup_pipeline();

#endif