#ifndef SIMULATORS_COMMON_H 
#define SIMULATORS_COMMON_H 

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <vector>
#include <string>

using namespace std;
namespace IDSIA{
  //#define MAX_MSG_SIZE 2048
#define MAX_MSG_SIZE 8192
  const int max_data = MAX_MSG_SIZE;


  typedef double Real;
  typedef signed short SInt16;
  typedef unsigned short UInt16;

  typedef signed int SInt32;
  typedef unsigned int UInt32;

  typedef signed long long SInt64;
  typedef unsigned long long UInt64;


  typedef struct
  {
    int sock;
    int addr_len;
    struct sockaddr_in server;
    struct sockaddr_in client;
    struct hostent *hp;  

  } Socket;

  typedef struct
  {
    int   id;
    Real x;
    Real y;
    Real z;
  
  } Position;


  typedef enum 
  {
    SIMULATION_INFORMATION,
    APPLICATION_DATA,
    RECEIVED_DATA,
    NODE_POSITIONS,
    START_FROM_ROBOT_PROCESS,
    NETWORK_SIM_INFORMATION,
    MSG_TYPE_NUM,
    STOP_SIMULATION
  } MsgType;


  typedef struct
  {
    MsgType type;
    int     robot_num;
    Real    simulation_step;
    Real    start_time;
    Real    stop_time;
    Real    arena_x;
    Real    arena_y;
    Real    arena_z;
    char    simulator_name[30];

  } SimulationInformationPacket;


  typedef struct
  {
    MsgType  type;
    Real     timestamp;
    int      robot_num;
    int      size;

  } NodePositionsPacket;


  typedef struct
  {
    MsgType type;
    Real   timestamp;
    int     from;
    int     to;
    int     size;
    int     delay;

  } ApplicationPacket;


  typedef struct
  {
    MsgType  type;
    int      id;
    char     robot_type[50];

  } RobotStartPacket;


  typedef struct
  {
    MsgType  type;
    char     network_sim[50];

  } NetworkSimInfoPacket; 
  
  typedef struct
  {
    MsgType  type;

  } StopSimulation;


  /***********************************************************************************/
  /***  Common functions for socket communications                    ***/
  /*                                                                    */
  /***********************************************************************************/
  void error(const char *msg);

  int OpenBlockingUDPSocketServerSide(Socket &new_socket, const int port);

  int OpenBlockingUDPSocketClientSide(Socket &new_socket, const int port, const char *server_host);

  int ServerSendDataToSocket(Socket &socket, const char *msg, const int msg_size);

  int ClientSendDataToSocket(Socket &socket, const char *msg, const int msg_size);

  int ServerReceiveDataFromSocketNonBlocking(Socket &socket, char **msg, int &msg_size, const int max_data);
  int ServerReceiveDataFromSocketNonBlocking(Socket &socket, char *msg, int &msg_size, const int max_data);

  int ServerReceiveDataFromSocketBlocking(Socket &socket, char **msg, int &msg_size, const int max_data);
  int ServerReceiveDataFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int max_data);

  int ServerPeekDataFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int max_data);
  int ServerPeekDataFromSocketNonBlocking(Socket &socket, char *msg, int &msg_size, const int max_data);

  int ServerReceiveFixedDataLenFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int data_len);

  int ClientReceiveDataFromSocketNonBlocking(Socket &socket, char **msg, int &msg_size, const int max_data);
  int ClientReceiveDataFromSocketNonBlocking(Socket &socket, char *msg, int &msg_size, const int max_data);

  int ClientReceiveDataFromSocketBlocking(Socket &socket, char **msg, int &msg_size, const int max_data);
  int ClientReceiveDataFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int max_data);

  int ClientPeekDataFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int max_data);

  int ClientReceiveFixedDataLenFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int data_len);


  /***********************************************************************************/
  /***  These functions are only used from the networks simulator side   ***/
  /*                                                                    */
  /***********************************************************************************/
  void BuildNetworkSimInformationPacket(char **msg, int &msg_size, const char *sim_name);
  void BuildNetworkSimInformationPacket(char *msg, int &msg_size, const char *sim_name);

  void GenerateRandomMsg(char **msg, int &msg_size, const int from, const int robot_num, const Real timestamp);
  void GenerateRandomMsg(char *msg, int &msg_size, const int from, const int robot_num, const Real timestamp);
  void GenerateMsg(char **msg, int &msg_size, const int from, const int to, const string content, const Real timestamp);

  /***********************************************************************************/

  /***********************************************************************************/
  /***  These functions are only used from the robot simulator side   ***/
  /*                                                                    */
  /***********************************************************************************/
  void BuildSimulationInformationPacket(char **msg, int &msg_size, const int robot_num, 
					const Real simulation_step, const Real simulation_time, 
					const char *sim_name, const Real stop_time);
  void BuildSimulationInformationPacket(char *msg, int &msg_size, const int robot_num, 
					const Real simulation_step, const Real simulation_time, 
					const char *sim_name, const Real stop_time);

  void BuildRobotStartPacket(char **msg, int &msg_size, const int id, const char *robot_type);
  void BuildRobotStartPacket(char *msg, int &msg_size, const int id, const char *robot_type);

  void BuildRobotPositionsPacket(char **msg, int &msg_size, const int robot_num, const Position *positions, const Real timestamp);
  void BuildRobotPositionsPacket(char *msg, int &msg_size, const int robot_num, const Position *positions, const Real timestamp);

  void GenerateRandomPositions(Position *positions, const int nodes_num, const Real xmax, const Real ymax, const Real zmax);
};
#endif
