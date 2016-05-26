#include "ns3-network-scheduler.h"
#include "ns3/ns3-network-application.h"


#include "getpot.h"
#include "CpuTime.h"

//#include "swarmanoidDefines.h"

#include "ns3/networkApplicationTag.h"



using namespace ns3;

int IDSIA::ns3NetworkScheduler::received[2] = {0,0};
int IDSIA::ns3NetworkScheduler::sended[2] = {0,0};
int IDSIA::ns3NetworkScheduler::sender[2] = {0,5};
int IDSIA::ns3NetworkScheduler::receiver[2] = {4,9};

void IDSIA::ns3NetworkScheduler::PhyTx(std::string con, Ptr<const Packet> p){
  NetworkApplicationTag nat;
  if(p->PeekPacketTag(nat)){
    std::cerr << "Sender: " << nat.GetSender();
    //std::cerr << " at phy layer: " << Simulator::Now().GetSeconds() << " s Created at " << nat.GetCreationTime().GetSeconds() << " size " << p->GetSize() << std::endl;
    std::cerr << " at phy layer: " << Simulator::Now().GetSeconds() << " s , size " << p->GetSize() << std::endl;
  }
}

IDSIA::ns3NetworkScheduler::ns3NetworkScheduler(std::string _configFilePath, std::string outputFp){
  this->configFilePath = _configFilePath;
  outputFilePath = outputFp;
  std::string input_file      = configFilePath;
  std::string base_directory  = "./";
  std::string comment_start   = "#";
  std::string comment_end     = "\n";
  std::string field_separator = ",";
  fConfigFile = new GetPot((base_directory+input_file).c_str(),
			  comment_start.c_str(),
			  comment_end.c_str(),
			  field_separator.c_str());
  int pb = (*fConfigFile)("argos/portBase",9999);
  port_base = pb;
//  if((*fConfigFile)("seed",10)==-1){
//    seed = time(NULL);
//  }else{
//    seed =(*fConfigFile)("seed",10);
//  }
  //seed = time(NULL);	// random seed
  seed = 12345;	// random seed
  printf("\n\nSEED: %d\n\n",seed);
  SeedManager::SetSeed (seed);
  msg_buf = new char[MAX_MSG_SIZE];
};


void IDSIA::ns3NetworkScheduler::gatherPositionInformation(){
  this->SendNetworkAck(step);
  this->GetNodePositionsUpdate();
  this->UpdateNodePositions();
  //scheduling the next position information update
  Simulator::Schedule(Seconds(this->GetSimulationTimeStep()), &IDSIA::ns3NetworkScheduler::gatherPositionInformation, this);
  static FILE* fp = fopen("test.dat","w");
  fprintf(fp,"%g;%d;%d;%d;%d;\n",Simulator::Now().GetSeconds(), sended[0], received[0],sended[1], received[1]);
}

int IDSIA::ns3NetworkScheduler::run(){
  apps.Start(Seconds(0.0));
  FILE* fp = fopen(outputFilePath.c_str(),"a");
  CpuTime *ct = new CpuTime();
  printf("Time - go!\n");
  ct->start();
  Simulator::Run();
  char stringa[256];
  sprintf(stringa, "Seed: %d number of nodes: %d", seed, numberOfNodes);
  ct->end();

  // save time statistics etc.

  fprintf(fp,"%d\t%f\t%f\t%f\t%f\t%d\n",numberOfNodes,ct->cpu_time_elapsed(),ct->user_time_elapsed(),ct->system_time_elapsed(),ct->total_time_elapsed(),seed);

  fclose(fp);
  printf("Time - stop!\n");
  fprintf(stderr,"Durata della simulazione (CPU): %g secondi\n", ct->cpu_time_elapsed());
  fprintf(stderr,"Durata della simulazione (total): %g secondi\n", ct->total_time_elapsed());
  Simulator::Destroy();
  return(0);
}

void IDSIA::ns3NetworkScheduler::CourseChange(std::string context, Ptr< const MobilityModel> model){
  Vector position = model->GetPosition();
  position.x = position.x;
  NS_LOG_UNCOND (context << "[" << Simulator::Now().GetSeconds() << "] " <<
    " x = " << position.x << ", y = " << position.y << ", z = " << position.z);
}


void IDSIA::ns3NetworkScheduler::UpdateNodePositions(){
  for(int i=0;i<numberOfNodes;i++){
    Ptr<ConstantPositionMobilityModel> mobility =  nodes.Get(i)->GetObject<ConstantPositionMobilityModel>();
    mobility->SetPosition(Vector(double(node_positions[i].x),double(node_positions[i].y),double(0.0)));
  }
}
int IDSIA::ns3NetworkScheduler::setup(){

  //Configuration file opening
  //check if the files esixts
  FILE *input_fp;
  // Check and open the input configuration file
  if( (input_fp = fopen(configFilePath.c_str(), "r")) == NULL ){
      perror("ERROR opening configuration file");
      exit(-1);
  }

  GetPot configFile(*fConfigFile);

  udpPort = configFile("simulation/udpPort",1025);
  printf("Opening socket\n");
  this->OpenSocketWithRobotSimScheduler();
 #if PRINT
  std::cerr << "Socket open, waiting for ARGoS" << std::endl;
 #endif
printf("Done, waiting for ARGoS\n");
  this->GetSimulationInformation();
printf("Obtained info from ARGoS.\n");

  nodes.Create(this->GetRobotNum());

  numberOfNodes = this->GetRobotNum();

  //setting the stop time of the simulation
  if(this->GetSimulationTime() != 0){
    Simulator::Stop(Seconds(this->GetSimulationTime()+this->GetSimulationTimeStep()/100));
  }
  /*x = configFile("simulation/scenario/x",-1.0);
    y = configFile("simulation/scenario/y",-1.0);*/
  x = GetArenaX();
  y = GetArenaY();

  if(x == -1 or y == -1){
    std::cerr << "Error, invalid scenario dimensions\n";
    exit(-1);
  }

  //setting up the mobility model
  MobilityHelper mobility;

  ObjectFactory pos;
  pos.SetTypeId("ns3::GridPositionAllocator");
  Ptr<PositionAllocator> posAll = pos.Create()->GetObject<PositionAllocator>();

  mobility.SetPositionAllocator(posAll);

  if(configFile("mobility/model","") == "argos"){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  }else{
    std::cerr << "Invalid mobility model supplied, available models are\n\targos\n";
    exit(-1);
  }
  mobility.Install(nodes);


  //////////////
  // Michal: using default parameter values !!!
  /////////////

  wifiMac = NqosWifiMacHelper::Default ();
  wifiMac.SetType ("ns3::AdhocWifiMac");
  wifiPhy = YansWifiPhyHelper::Default ();

//  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
//  wifiChannel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel");

  	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  	wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(2.0));

  wifiPhy.SetChannel (wifiChannel.Create ());
  wifi = WifiHelper::Default ();
  wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
  //wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "NonUnicastMode",StringValue("DsssRate2Mbps"), "DataMode",StringValue("DsssRate2Mbps"),"ControlMode",StringValue("DsssRate2Mbps"),"RtsCtsThreshold", UintegerValue (0));
  devices = wifi.Install(wifiPhy, wifiMac, nodes);








  //wifiPhy.EnablePcapAll (std::string ("aodv"));


  //Routing
  std::string routingProtocol = configFile("routing/protocol","None");
  AodvHelper routing;



  if(routingProtocol == "AODV"){

	  //////////////
	  // Michal: using default parameter values !!!
	  /////////////

//    configFile.set_prefix("routing/protocol/aodv/");
//    routing.Set("HelloInterval", TimeValue(NanoSeconds(configFile("HelloInterval", 1000000000.0))));
//    routing.Set("RreqRetries", UintegerValue(configFile("RreqRetries", 2)));
//    routing.Set("RreqRateLimit", UintegerValue(configFile("RreqRateLimit",10)));
//    routing.Set("NodeTraversalTime", TimeValue(NanoSeconds(configFile("NodeTraversalTime",40000000.0))));
//    routing.Set("NextHopWait", TimeValue(NanoSeconds(configFile("NextHopWait",50000000.0))));
//    routing.Set("ActiveRouteTimeout",TimeValue(NanoSeconds(configFile("ActiveRouteTimeout",3000000000.0))));
//    routing.Set("MyRouteTimeout",TimeValue(NanoSeconds(configFile("MyRouteTimeout",11199999999.0))));
//    routing.Set("BlackListTimeout",TimeValue(NanoSeconds(configFile("BlackListTimeout",5599999999.0))));
//    routing.Set("DeletePeriod", TimeValue(NanoSeconds(configFile("DeletePeriod",15000000000.0))));
//    routing.Set("TimeoutBuffer",UintegerValue(configFile("TimeoutBuffer",2)));
//    routing.Set("NetDiameter",UintegerValue(configFile("NetDiameter",35)));
//    routing.Set("NetTraversalTime",TimeValue(NanoSeconds(configFile("NetTraversalTime",2799999999.0))));
//    routing.Set("PathDiscoveryTime",TimeValue(NanoSeconds(configFile("PathDiscoveryTime",5599999999.0))));
//    routing.Set("MaxQueueLen",UintegerValue(configFile("MaxQueueLen",64)));
//    routing.Set("MaxQueueTime",TimeValue(NanoSeconds(configFile("MaxQueueTime",30000000000.0))));
//    routing.Set("AllowedHelloLoss",UintegerValue(configFile("AllowedHelloLoss",2)));
//    routing.Set("GratuitousReply",BooleanValue(configFile("GratuitousReply",true)));
//    routing.Set("DestinationOnly",BooleanValue(configFile("DestinationOnly",false)));
//    routing.Set("EnableHello",BooleanValue(configFile("EnableHello", true)));
//    routing.Set("EnableBroadcast",BooleanValue(configFile("EnableBroadcast",true)));
//    configFile.set_prefix("");
  }else{
    std::cerr << "Invalid routing protocol, available protocols are\n\tAODV\n";
    exit(-1);
  }
  stack.SetRoutingHelper(routing);
  stack.Install(nodes);

  if(numberOfNodes < 65534){
    addr.SetBase("10.0.0.0", "255.0.0.0");
  }else{
    std::cerr << "Too many nodes in simulations, maximal allowed nodes are 65534" << std::endl;
    exit(-1);
  }

  interfaces = addr.Assign(devices);



  //Setting up the applications

  this->appSetup();

  current_time_slice = this->GetCurrentTimeSlice();

  Simulator::Schedule(Seconds(0.0),&IDSIA::ns3NetworkScheduler::sendAckAndBasicInfoFromRobots,this);
  //schedule the first position reading
  Simulator::Schedule(Seconds(0.0), &IDSIA::ns3NetworkScheduler::gatherPositionInformation, this);

  //connecting the Tx and Rx trace sources
  std::ostringstream oss;
  oss << "/NodeList/*/ApplicationList/*/$IDSIA::NetworkApplication/Tx";
  Config::Connect(oss.str(), MakeCallback(&IDSIA::ns3NetworkScheduler::Tx, this));

  std::ostringstream oss2;
  oss2 << "/NodeList/*/ApplicationList/*/$IDSIA::NetworkApplication/Rx";
  Config::Connect(oss2.str(), MakeCallback(&IDSIA::ns3NetworkScheduler::Rx, this));

  return(0);
}

void IDSIA::ns3NetworkScheduler::appSetup(){
  //std::cerr << this->GetSimulationTimeStep() << std::endl;
  appHelper = new NetworkApplicationHelper(port_base, numberOfNodes, this->GetSimulationTimeStep(), udpPort, this->interfaces);
  apps = appHelper->Install(nodes);

  //
}

void IDSIA::ns3NetworkScheduler::sendAckAndBasicInfoFromRobots(void){
  for(ApplicationContainer::Iterator i=apps.Begin();i!=apps.End();i++){
    Ptr<NetworkApplication> a;
    a=DynamicCast<NetworkApplication>(*i);
    a->InitSocketWithRobotApplication();
  }
#if PRINT
  //sending ack back to the robot simulator
  std::cerr << "Sending ACK to Argos\n";
#endif
  this->SendNetworkSimInfo((char*)"ns3/NetworkScheduler");
  for(ApplicationContainer::Iterator i=apps.Begin();i!=apps.End();i++){
    Ptr<NetworkApplication> a;
    a=DynamicCast<NetworkApplication>(*i);
    a->GetStartInfoFromRobotAppl();
  }


}



void IDSIA::ns3NetworkScheduler::OpenSocketWithRobotSimScheduler()
  {
    OpenBlockingUDPSocketServerSide(socket, port_base);
  };


void IDSIA::ns3NetworkScheduler::GetSimulationInformation()
  {
    ServerReceiveDataFromSocketBlocking(socket, (char *)&sim_info, msg_size, sizeof(SimulationInformationPacket));

    if( msg_size != sizeof(SimulationInformationPacket) )
      {
	fprintf(stderr, "ERROR READING SIMULATION INFORMATION HEADER (%d bytes instead of %lu)\n", msg_size, sizeof(SimulationInformationPacket));
	exit(-1);
      }

    if( sim_info.type != SIMULATION_INFORMATION )
     {
       fprintf(stderr, "WRONG TYPE FOR MSG SIM_INFO: %d\n", sim_info.type);
       exit(-1);
     }
    node_positions = new Position[sim_info.robot_num];
    current_time   = sim_info.start_time;
  };


void IDSIA::ns3NetworkScheduler::SendNetworkSimInfo(char *sim_name)
  {
    BuildNetworkSimInformationPacket((char *)&msg_net_info, msg_size, sim_name);

    ServerSendDataToSocket(socket, (char *)&msg_net_info, msg_size);
  };


void IDSIA::ns3NetworkScheduler::SendNetworkAck(int step)
{
  char msg = (char)step;
  ServerSendDataToSocket(socket, &msg, 1);
};


void IDSIA::ns3NetworkScheduler::GetNodePositionsUpdate()
  {

    ServerPeekDataFromSocketBlocking(socket, (char *)&msg_positions, msg_size, sizeof(NodePositionsPacket));
    if( msg_size != sizeof(NodePositionsPacket) )
      {
	fprintf(stderr, "ERROR READING POSITIONS HEADER (%d bytes instead of %lu)\n", msg_size, sizeof(NodePositionsPacket));
	exit(-1);
      }

    /*Check if simulation has to be stopped*/
    if(msg_positions.type == STOP_SIMULATION){
      Simulator::Stop();
      return;
    }
    if( msg_positions.type  != NODE_POSITIONS )
      {
	fprintf(stderr, "WRONG TYPE FOR NODE POSITIONS HEADER: %d\n", msg_positions.type);
	exit(-1);
      }


    updated_positions = msg_positions.robot_num;
    current_time      = msg_positions.timestamp;
    ServerReceiveDataFromSocketBlocking(socket, (char **)&msg_buf, msg_size, sizeof(NodePositionsPacket) + msg_positions.size);
    if((uint)msg_size != msg_positions.size + sizeof(NodePositionsPacket))
      {
	fprintf(stderr, "ERROR READING POSITIONS (%d bytes instead of %lu)\n", msg_size, sizeof(NodePositionsPacket) + msg_positions.size);
	exit(-1);
      }
    else
      {
	memcpy(node_positions, msg_buf + sizeof(NodePositionsPacket), msg_positions.size);
      }
      //exit(-1);
  }

void IDSIA::ns3NetworkScheduler::Rx(std::string con, Ptr<const Packet> p){
}

void IDSIA::ns3NetworkScheduler::Tx(std::string con, Ptr<const Packet> p){
}
