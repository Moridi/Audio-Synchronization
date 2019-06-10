#include "Server.h"

int main(int argc, char *argv[])
{
  setup_client(argc, argv, 0);
  setup_pipeline();
  return 0;
}