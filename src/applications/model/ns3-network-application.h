/**
 * Appliaction to simulate the robot behavior on the network side
 */
#ifndef _NS3_NETWORK_APPLICATION_H
#define _NS3_NETWORK_APPLICATION_H


//Common includes
#include <vector>
#include <string>

/*IDSIA includes (same folder)*/
#include "ns3/simulators-common.h"


/* ns3 includes */
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/application.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/traced-callback.h"
#include "ns3/random-variable-stream.h"
using namespace ns3;
using namespace std;

class RandomVariable;


namespace IDSIA{

  class NetworkApplication : public Application{
    //required by Application and / or Object
  public:
    static TypeId GetTypeId(void);
    static std::map< Ipv4Address, int> AddrToRobot;
    static std::map< int, Ipv4Address> RobotToAddr;
    //NetworkApplication(uint16_t numberOfRobots, uint16_t ns3Port, float simulationStep);
    NetworkApplication();
    virtual ~NetworkApplication();
  private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);
    Ipv4Address GetRobotAddress(uint32_t);
    uint32_t GetRobotId(const Ipv4Address &);
  protected:
    virtual void DoDispose(void);

    //own stuff

  protected:
    //fields
    Ptr<ns3::Socket> sendSocket;
    Ptr<ns3::Socket> sinkSocket;
    uint16_t numberOfRobots;
    uint16_t port;
    IDSIA::Socket robot2ns3Socket;
    Real simulationStep;

    Ptr<ns3::ConstantRandomVariable> m_onTime, m_offTime;
    Ptr<ns3::UniformRandomVariable> m_txDelay;



    //fields derivati da NetworkApplication sviluppata da Gianni
    RobotStartPacket msg_start;
    ApplicationPacket msg_header;
    char *msg_content;
    int msg_size;
    int max_content_size;
    char *received_data;
    uint16_t appId;
    uint16_t portBase; //porta usata dallo scheduler!

    Ipv4InterfaceContainer interfaces;
    vector<Ptr<ns3::Socket> > socketVector;
    //a trace source to detect transmitted packages
    TracedCallback<Ptr<const Packet> > m_txTrace;

    //a trace source to detect received  packages
    TracedCallback<Ptr<const Packet> > m_rxTrace;
    //methods
    void Receive(Ptr<ns3::Socket> socket); //Callback per la ricezione
    void Sent(Ptr<ns3::Socket> socket,  uint32_t length); //callback x invio (serve x debug)
    void ScheduleNextMsgReading(void);
    void CreateAndSendPacket(std::string content, int destinationId, int delay);
    void DelayedSendSocket(Ptr<Packet> packet, Ipv4Address destination,uint16_t port);



 //methods derivati da NetworkApplication di Gianni
  public:
    void InitSocketWithRobotApplication();
    void GetStartInfoFromRobotAppl();
    vector<list<std::pair<string, int> > >
      GetNewInputDataFromRobotApp(Real current_time_slice, int num_robots);
    void ReceiveData(Real current_time_slice);
    void SendReceivedDataToRobotApp(Real current_time_slice, int from, string content);
    inline IDSIA::Socket GetSocket(){return robot2ns3Socket; }
    void AcquireAndSendMessages(void);
    void SetInterfaceContainer(Ipv4InterfaceContainer inter){this->interfaces = inter;}
  protected:
    //Servono internamente


    //Field statici

    static uint16_t staticId;//Numero di apps istanziate --> serve da aggiungere a portbase
  };
}

#endif
/* _NS3-NETWORK-APPLICATION_H_ */
