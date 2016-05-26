#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
// To enable visualizations...
//#include "ns3/pyviz.h"


#include "ns3-network-scheduler.h"

int main(int argc, char **argv){
	  // To enable visualizations ()...
  //	  { PyViz v; }

  CommandLine cmd;
  cmd.Parse(argc,argv);
  IDSIA::ns3NetworkScheduler *scheduler = new IDSIA::ns3NetworkScheduler(argv[1],argv[2]);
  scheduler->setup();
  scheduler->run();
  return(0);
}
