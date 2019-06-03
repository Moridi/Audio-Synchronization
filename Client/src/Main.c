#include "Client.h"

int main(int argc, char *argv[])
{
    initialize_pipeline(argc, argv);
    add_decoder();
    add_rtpbin();
    link_all_to_rtpbin();
    get_rtcp_sink_pad();
    get_rtcp_source_pad();
    connect_signals();
    setup_pipeline();

    return 0;
}