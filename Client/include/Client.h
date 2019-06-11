#ifndef CLIENT_H_
#define CLIENT_H_

void initialize_pipeline(int argc,
        char *argv[], int client_id);
void add_decoder();
void add_rtpbin();
void link_all_to_rtpbin();
void get_rtcp_sink_pad();
void get_rtcp_source_pad();
void connect_signals();
void setup_pipeline();
void setup_client();

#endif