#include "Server.h"

int main(int argc, char *argv[])
{
  initialize_pipeline(argc, argv);
  add_rtpbin();
  setup_client(argc, argv, 0);
  
  setup_pipeline();
  return 0;
}