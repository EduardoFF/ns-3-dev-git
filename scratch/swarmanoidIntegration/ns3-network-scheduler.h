/**
 * Implementazione di una classe per estendere network-simulator
 * in modo da inserire i vari binding necessari per collegare quanto fatto
 * da Gianni con il simulatore ns3 ed evitare di riscrivere codice inutilmente
 * fornisce inoltre tutto il necessario per far girare la simulazione in ns3
 * quindi la creazione dei nodi, la gestione della mobilità
 * */


//import ns3
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/aodv-module.h"

//#include "ns3/common-module.h"
//#include "ns3/simulator-module.h"
#include "ns3/mobility-module.h"
//#include "ns3/helper-module.h"
//#include "ns3/node-module.h"
#include "ns3/wifi-module.h"
#include "ns3/socket.h"
//fine import ns3

//import std library
#include <set>
#include <vector>
#include <string>
//fine import std library


//import IDSIA
#include "simulators-common.h"
//#include "ns3-network-application.h"
#include "ns3/ns3-network-application-helper.h"
//libs
//#include "draw.h"
//fine import IDSIA


class GetPot;

// FOR CLICK ONLY
class TxCallbackWrapper;

using namespace ns3;


//namespace IDSIA per evitare errori di import

namespace IDSIA{


  class ns3NetworkScheduler{

  public:
    //public stuff

    ns3NetworkScheduler(std::string _configFilePath, std::string outFilePath);
    int run();
    int setup();

  protected:
    //fields
    //idsia_draw::Image win;
    double *positions;
    int numberOfNodes;
    Real scaleX; //scala x la visualizzazione dell'arena
    Real scaleY;
    double x,y; //dimensione dell'arena
    double numberOfSquares;//Numero di quadrati visualizzati nella griglia della finestra grafica
    ns3::NodeContainer nodes;
    WifiHelper wifi;
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;
    NqosWifiMacHelper wifiMac;
    NetDeviceContainer devices;
    InternetStackHelper stack;
    // Ptr<AodvHelper> routing;
    Ipv4AddressHelper addr;
    Ipv4InterfaceContainer interfaces;
    int step;
    int udpPort;
    IDSIA::Socket socket;
    IDSIA::SimulationInformationPacket sim_info;
    ApplicationContainer apps;
    NetworkApplicationHelper *appHelper;
    std::string configFilePath;//Path to the configuration file, for easier changing of the scenario
    std::string outputFilePath;//Path to the output file with time statistics etc.
    GetPot *fConfigFile;
    float current_time_slice;
    int seed;

    static int received[];
    static int sended[];
    static int sender[];
    static int receiver[];

    //methods
    void gatherPositionInformation();
    void UpdateNodePositions();
    void CourseChange(std::string context, Ptr< const MobilityModel> model);
    //void draw();
    void appSetup();
    void GetApplicationListWithDataReady();

    void sendAckAndBasicInfoFromRobots(void);
    void Rx(std::string con, Ptr<const Packet> p);
    void Tx(std::string con, Ptr<const Packet> p);
    void PhyTx(std::string con, Ptr<const Packet> p);

    void OpenSocketWithRobotSimScheduler();
    void GetSimulationInformation();
    void SendNetworkSimInfo(char *sim_name);
    void GetNodePositionsUpdate();
    void SendNetworkAck(int step);

    int GetSimulationTime(){ return sim_info.stop_time; }
    int  GetRobotNum() { return sim_info.robot_num; }
    Real GetSimulationTimeStep() { return sim_info.simulation_step; }
    char *GetRobotSimulatorName() { return sim_info.simulator_name; }
    Real GetCurrentTimeSlice() { return current_time; }
    Real GetArenaX(){ return sim_info.arena_x; };
    Real GetArenaY(){ return sim_info.arena_y; };
    inline Socket GetSocket() { return socket; }

  protected:

    int port_base;
    int msg_size;

    NetworkSimInfoPacket msg_net_info;
    NodePositionsPacket msg_positions;
    Real current_time;
    char *msg_buf;


    int updated_positions;
    Position *node_positions;

    // FOR CLICK ONLY
    std::vector<TxCallbackWrapper*> callbackWrappers;
  };

};
