#include "Server.h"

int main(int argc, char *argv[])
{
  initialize_pipeline(argc, argv);
  add_rtpbin();
  add_udp_terminals();
  link_to_rtpbin();
  get_rtp_source_pad();
  get_rtcp_source_pad();
  receive_rtcp();
  setup_pipeline();

  return 0;
}