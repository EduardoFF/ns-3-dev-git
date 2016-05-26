#include "ns3-network-application.h"

#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/nstime.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/inet-socket-address.h"
#include "ns3/application.h"
#include "ns3/socket-factory.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
//#include "ns3/random-variable.h"
//#include "ns3/object.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/udp-socket.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"

//#include "swarmanoidDefines.h"
#include "networkApplicationTag.h"
#include "ns3/trace-source-accessor.h"
//#include "ns3/random-variable.h"

/// to enable some debugging info
#define __NETWORKAPP_REPORT 0
/// to enable even more debugging info
#define __NETWORKAPP_DEBUG 0



using namespace ns3;
using namespace std;
namespace IDSIA{

  uint16_t NetworkApplication::staticId = 0;
  std::map< Ipv4Address, int> NetworkApplication::AddrToRobot;
  std::map< int, Ipv4Address> NetworkApplication::RobotToAddr;

  NS_OBJECT_ENSURE_REGISTERED (NetworkApplication);
  NS_LOG_COMPONENT_DEFINE("NetworkApplication");
  TypeId NetworkApplication::GetTypeId(void){
    static TypeId tid = TypeId("IDSIA::NetworkApplication")
      .SetParent<Application>()
      .AddConstructor<NetworkApplication>()
      .AddAttribute("portBase", "Port number on which the schedulers are connected",
		    UintegerValue(1025),
		    MakeUintegerAccessor(&NetworkApplication::portBase),
		    MakeUintegerChecker<uint16_t>(1))
      .AddAttribute("numberOfRobots", "The number of robots present in the simulation",
		    UintegerValue(10),
		    MakeUintegerAccessor(&NetworkApplication::numberOfRobots),
		    MakeUintegerChecker<uint16_t>(1))
      .AddAttribute("port", "Port number on which the ns3 <-> ns3 socket comunicates",
		    UintegerValue(1025),
		    MakeUintegerAccessor(&NetworkApplication::port),
		    MakeUintegerChecker<uint16_t>(1))
      .AddAttribute("simulationStep", "The simulation step used to synchronize Argos and ns3",
		    DoubleValue(0.1),
		    MakeDoubleAccessor(&NetworkApplication::simulationStep),
		    MakeDoubleChecker<double>(0.001))
      .AddTraceSource ("Tx", "A new packet is created and is sent",
		       MakeTraceSourceAccessor (&NetworkApplication::m_txTrace))
      .AddTraceSource ("Rx", "A new packet is received",
		       MakeTraceSourceAccessor (&NetworkApplication::m_rxTrace));

    return tid;
  }



  NetworkApplication::NetworkApplication(){
    //std::cerr << "StaticId = " << staticId << std::endl;
    appId = staticId++;
    //std::cerr << "appId = " << appId << std::endl;
    max_content_size = MAX_MSG_SIZE;


    //setting the 2 randomvariable generators

    m_onTime = CreateObject<ConstantRandomVariable> ();
    m_offTime = CreateObject<ConstantRandomVariable> ();
    
    m_offTime->SetAttribute ("Constant", DoubleValue (0.0));
    m_onTime->SetAttribute ("Constant", DoubleValue (1.0));
						    
	
    //    m_onTime = ConstantVariable(1.0);
    //m_offTime = ConstantVariable(0.0);
    //    m_txDelay = UniformVariable(1000,10e5); /// range: 1 ms upto 100 sec
    m_txDelay = CreateObject<UniformRandomVariable> ();
    m_txDelay->SetAttribute ("Min", DoubleValue (1000));
    m_txDelay->SetAttribute ("Max", DoubleValue (10e5));
    //this->InitSocketWithRobotApplication(); //Non funziona, Binding: Permission Denied, porta nn settata
  }
  NetworkApplication::~NetworkApplication(){}

  void NetworkApplication::StartApplication(void)
  {
    sendSocket = ns3::Socket::CreateSocket(GetNode(),TypeId::LookupByName("ns3::UdpSocketFactory"));
    sendSocket->Bind();
    sendSocket->SetRecvCallback(MakeCallback(&IDSIA::NetworkApplication::Receive,this));
    sendSocket->SetSendCallback(MakeCallback(&IDSIA::NetworkApplication::Sent,this));
    bool sendBroadcast =  sendSocket->GetAllowBroadcast();
    if (!sendBroadcast)
      sendSocket->SetAllowBroadcast (true);

    sinkSocket = ns3::Socket::CreateSocket(GetNode(), TypeId::LookupByName("ns3::UdpSocketFactory"));
    Ipv4Address dstaddr = GetRobotAddress(this->appId);
    InetSocketAddress dst = InetSocketAddress(dstaddr,port);
    sinkSocket->Bind(dst);
    sinkSocket->SetRecvCallback(MakeCallback(&IDSIA::NetworkApplication::Receive,this));
    sinkSocket->SetSendCallback(MakeCallback(&IDSIA::NetworkApplication::Sent,this));
    bool receiveBroadcast = sinkSocket->GetAllowBroadcast();
    if (!receiveBroadcast)
      sinkSocket->SetAllowBroadcast (true);

    //calling the rng
    m_onTime->GetValue();
    m_offTime->GetValue();
    fprintf(stderr, "StartApplication OK\n");
    fflush(stderr);

  }

  void NetworkApplication::StopApplication(void){
    if(sendSocket != NULL){
      //sinkSocket->SetRecvCallback(MakeCallback(NULL));//setting the recv callback to null
      sendSocket->SetSendCallback((Callback<void, Ptr<ns3::Socket>, uint32_t>) NULL); //setting the send callback to null
    }
  }

  void NetworkApplication::DoDispose(void){
    //close the ns3 socket
    if(sinkSocket != NULL){
      sinkSocket->Close();
    }
    if(sendSocket != NULL){
      sendSocket->Close();
    }
    //sending the call up
    ns3::Application::DoDispose();
  }


  void NetworkApplication::Receive(Ptr<ns3::Socket> socket){
#if __NETWORKAPP_REPORT
    std::cerr << Simulator::Now().GetSeconds() << " Node " << appId << " received \n" ;
#endif
    //estrapolazione del contenuto del pacchetto
    Address from;
    Ptr<Packet> packet = sinkSocket->RecvFrom(from);
    uint8_t buf[packet->GetSize()];
    packet->CopyData(buf,packet->GetSize());
    m_rxTrace (packet);
    //std::cerr << "\tReceived Msg len " << packet->GetSize() << " is: " << buf << std::endl;

    InetSocketAddress address = InetSocketAddress::ConvertFrom(from);
    int32_t src  = GetRobotId(address.GetIpv4());
    //for(uint32_t i = 0; i<interfaces.GetN();i++){
    //if(GetRobotAddress(i) == address.GetIpv4()){
    //src = i;
    //break;
    //}
    //}
    //std::cerr << "From Node: " << src << std::endl;
    //printf("[%f] %d received msg from %d\n",Simulator::Now().GetSeconds(),sinkSocket->GetNode()->GetId(),src);
    std::string str_content((const char*)buf,packet->GetSize());
    SendReceivedDataToRobotApp(Simulator::Now().GetSeconds(),src, str_content);
  }


  void NetworkApplication::Sent(Ptr<ns3::Socket> socket,  uint32_t length){ //callback x invio (serve x debug)
    //std::cerr << Simulator::Now().GetSeconds() << " Socket sended " << length << " bytes\n";
  }


  void NetworkApplication::ScheduleNextMsgReading(void){
    //this->ReadMsgFromArgos();
  }

  void NetworkApplication::InitSocketWithRobotApplication(){
    //this->max_content_size = _max_content_size;

    //apertura del socket verso il robot
#if __NETWORKAPP_DEBUG
    std::cerr << "Node: " << this->appId << " / " << GetNode() << " / " << this
	      << " opening connection to port: " << portBase+(appId+1)
	      << " portBase: " << portBase << std::endl;
#endif
    OpenBlockingUDPSocketServerSide(robot2ns3Socket,portBase+(appId+1));
    this->msg_content = new char[this->max_content_size];
    this->received_data = new char[max_content_size];
  }


  void NetworkApplication::GetStartInfoFromRobotAppl(){
#if __NETWORKAPP_DEBUG
    std::cerr << "Time: " << Simulator::Now().GetSeconds() << " waiting on robot2ns3 socket, node: " << appId << std::endl;
#endif
    ServerReceiveDataFromSocketBlocking(robot2ns3Socket,(char *)&msg_start, msg_size, sizeof(RobotStartPacket));
    //std::cerr << "Node: " << appId << " received back\n";
    //Controllo errori
    if(msg_size != sizeof(RobotStartPacket))
      {
      fprintf(stderr, "WRONG SIZE FROM ROBOT START PACKET (size %d instead of %lu\n", msg_size, sizeof(RobotStartPacket));
      fflush(stderr);
      exit(-1);
    }
    if(msg_start.type != START_FROM_ROBOT_PROCESS)
      {
      fprintf(stderr,"WRONG START PACKET (type %d)\n", msg_start.type);
      fflush(stderr);
      exit(-1);
    }
    else{
#if __NETWORKAPP_REPORT
      fprintf(stderr, "[%d] Received START from robot %d (%s)\n", appId, msg_start.id, msg_start.robot_type);
#endif
    }
    //scheduling the first search for new msgs
    Simulator::Schedule(Seconds(0.0), &NetworkApplication::AcquireAndSendMessages, this);
  }

  Ipv4Address 
  NetworkApplication::GetRobotAddress(uint32_t robotId)
  {
    if( NetworkApplication::RobotToAddr.size() )
      {
	std::map<int, Ipv4Address>::iterator it =
	  RobotToAddr.find(robotId);
	if( it == RobotToAddr.end() )
	  {
	    fprintf(stderr,"Can not find robot %d ip address in address book\n",
		    robotId);
	    return Ipv4Address();
	  }
	return it->second;
      }
    interfaces.GetAddress(robotId);
  }

  uint32_t
  NetworkApplication::GetRobotId(const Ipv4Address &ipaddr)
  {
    if( NetworkApplication::AddrToRobot.size() )
      {
	std::map<Ipv4Address, int>::iterator it =
	  AddrToRobot.find(ipaddr);
	if( it == AddrToRobot.end() )
	  {
	    std::cerr << "Can not find robot id for address "
		      << ipaddr << "in address book\n";
	    return -1;
	  }
	return it->second;
      }
    else
      {
	/// this will return -1 if address is not found
	uint32_t rid = -1;
	for(uint32_t i = 0; i<interfaces.GetN();i++){
	  if(interfaces.GetAddress(i) == ipaddr){
	    rid = i;
	    break;
	  }
	}
	return rid;
      }
    return -1;
  }

  vector<list<std::pair<string,int> > >
  NetworkApplication::GetNewInputDataFromRobotApp(Real current_time_slice,
						  int num_robots)
  {
    char num_msgs;
    vector< list< std::pair<string, int> > > vecRecv(num_robots);
    ServerReceiveDataFromSocketBlocking(robot2ns3Socket, &num_msgs, msg_size, 1);
#if __NETWORKAPP_DEBUG
    fprintf(stderr, "Node %d receiving %d messages from robot simulator\n", appId, num_msgs);
#endif
    for(int i=0;i<num_msgs;i++)
      {
	ReceiveData(current_time_slice);
	if(msg_header.to == -1)
	  {
	    /*-1  means a broadcast message*/
	    msg_header.to = this->appId;/*A message to yourself means broadcast*/
	  }
	//	std::cerr << "inputData size " << msg_size-sizeof(ApplicationPacket) << std::endl;
	string str_content(msg_content + sizeof(ApplicationPacket),msg_size-sizeof(ApplicationPacket));
	vecRecv[msg_header.to].push_back(std::make_pair(str_content,msg_header.delay));
      }
    return vecRecv;
  }


  void NetworkApplication::ReceiveData(Real current_time_slice)
  {
    ServerReceiveDataFromSocketBlocking(robot2ns3Socket,
					&msg_content,
					msg_size,
					MAX_MSG_SIZE + sizeof(ApplicationPacket));
    if( msg_size < sizeof(ApplicationPacket) )
      {
	printf("wrong msg_size %d\n", msg_size);
	exit(1);
      }
    memcpy(&msg_header, msg_content, sizeof(ApplicationPacket));
#if __NETWORKAPP_DEBUG
    fprintf(stderr, "[%d @ %g] Received an APPLICATION Msg sent from robot %d at %g to robot %d (size %d bytes) msg_size %d\n",
	    appId, current_time_slice, msg_header.from, msg_header.timestamp, msg_header.to, msg_header.size, msg_size);
    //    fprintf(stderr, "\t Msg content: [%s]\n", msg_content+sizeof(ApplicationPacket));
#endif
  }

  void NetworkApplication::SendReceivedDataToRobotApp(Real current_time_slice, int from, string content){
    GenerateMsg(&received_data, msg_size, from, appId, content, current_time_slice);
    ServerSendDataToSocket(robot2ns3Socket, received_data, msg_size);
  }

  void NetworkApplication::AcquireAndSendMessages(void)
  {
    std::vector<list<std::pair< std::string, int> > > vecMsgs;
    Real current_time_slice = (float)Simulator::Now().GetSeconds();
    vecMsgs = this->GetNewInputDataFromRobotApp(current_time_slice, numberOfRobots);
    //Iterating over the messages
    for(uint32_t i=0;i<numberOfRobots;i++)
      {
      if(!vecMsgs.at(i).empty())
	{
#if __NETWORKAPP_DEBUG
	std::cerr << Simulator::Now().GetSeconds() << " Node " << appId
		  << " has new msg to send to " << i << std::endl;
#endif
	for(list<std::pair< string, int> >::iterator k=vecMsgs.at(i).begin();
	    k!=vecMsgs.at(i).end();k++)
	  {
	    this->CreateAndSendPacket(k->first,i,k->second);
	  }

      }
    }
    //scheduling the next acquiring and sending operation
    Simulator::Schedule(Seconds(this->simulationStep),&IDSIA::NetworkApplication::AcquireAndSendMessages, this);
  }

  void
  NetworkApplication::CreateAndSendPacket(std::string content, int destinationId,
					       int delay)
  {
#if __NETWORKAPP_DEBUG
    std::cerr << "Time (s): " << Simulator::Now().GetSeconds() << " Node " << this->appId << " ready to Send\n";
#endif
    //creating the packet to send over the network
    Ptr<Packet> packet = Create<Packet> (reinterpret_cast<const uint8_t*>(content.c_str()),content.size()+1);
    //uint32_t sizeGap = PACKETSIZE - packet->GetSize();
    //packet->AddPaddingAtEnd(sizeGap);			// TODO FIXME to add here or not - make a decision !!!!!!
    //std::cerr << "packet->GetSize(): " << packet->GetSize() << std::endl;
    NetworkApplicationTag nat;
    nat.SetSender(this->appId);
    packet->AddPacketTag(nat);
    //Getting the address of the destination
    Ipv4Address destination;
    if(destinationId == appId)
      {
	destination = GetRobotAddress(destinationId).GetBroadcast();
      }
    else{
      destination = GetRobotAddress(destinationId);
    }
    //#if __NETWORKAPP_DEBUG
    //#endif
    //sending (Michal added: with delay, as many robots sending at _exactly_ the same time may cause unexpected interferences...)
    unsigned long int  f_delay_us = delay*1000 + m_txDelay->GetInteger(0,999);
#if __NETWORKAPP_DEBUG
    std::cerr << "Destination Address is: "
	      << destinationId
	      << " address: "
	      << destination 
	      << " " <<  packet->GetSize()
	      << " Node " << this->appId
	      << " delay " << f_delay_us << std:: endl;
#endif
    Simulator::Schedule (MicroSeconds(f_delay_us),
			 &NetworkApplication::DelayedSendSocket, this, packet,destination,port);
    //int bytes = sendSocket -> SendTo(packet, 0, InetSocketAddress(destination,port));
    //printf("[%f]%d sent a packet to %d\n",Simulator::Now().GetSeconds(),appId,destinationId);
    //#if __NETWORKAPP_DEBUG
    //std::cerr << " Sent " << packet->GetSize() << " bytes\n";
    //#endif
    m_txTrace (packet);

  }

  void NetworkApplication::DelayedSendSocket(Ptr<Packet> packet, Ipv4Address destination,uint16_t port) {
    sendSocket -> SendTo(packet, 0, InetSocketAddress(destination,port));
  }

}



