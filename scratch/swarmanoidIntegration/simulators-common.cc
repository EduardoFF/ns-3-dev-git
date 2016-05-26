#include "simulators-common.h"

/***********************************************************************************/
/***  Common functions for socket communications                    ***/
/*                                                                    */
/***********************************************************************************/
void IDSIA::error(const char *msg)
{
    perror(msg);
    exit(-1);
}

int IDSIA::OpenBlockingUDPSocketServerSide(Socket &new_socket, const int port)
{
  if( (new_socket.sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    error("Opening socket");
  
  new_socket.addr_len = sizeof(struct sockaddr_in);
  bzero(&new_socket.server, new_socket.addr_len);
  
  new_socket.server.sin_family      = AF_INET;
  new_socket.server.sin_addr.s_addr = INADDR_ANY;
  new_socket.server.sin_port        = htons(port);
  
  if ( bind(new_socket.sock,(struct sockaddr *)&new_socket.server, new_socket.addr_len) < 0 ) 
    error("Binding");
  
  return new_socket.sock;
}


int IDSIA::OpenBlockingUDPSocketClientSide(Socket &new_socket, const int port, const char *server_host)
{
  if( (new_socket.sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    error("Opening socket");
  
  new_socket.server.sin_family = AF_INET;

  if ( (new_socket.hp = gethostbyname(server_host)) == 0 )
    error("Unknown host");
  
  bcopy((char *)new_socket.hp->h_addr, (char *)&new_socket.server.sin_addr, new_socket.hp->h_length);
  new_socket.server.sin_port = htons(port);

  new_socket.addr_len = sizeof(struct sockaddr_in);

  return new_socket.sock;
}


int IDSIA::ServerSendDataToSocket(Socket &socket, const char *msg, const int msg_size)
{
  //fprintf(stderr, "Server sending %d bytes to socket %d\n", msg_size, socket.sock);
  int r = sendto(socket.sock, msg, msg_size, 0, (struct sockaddr *)&socket.client, socket.addr_len);
  if( r != msg_size )
    error("Server send");
  return r;
}


int IDSIA::ClientSendDataToSocket(Socket &socket, const char *msg, const int msg_size)
{
  //fprintf(stderr, "Client sending %d bytes to socket %d\n", msg_size, socket.sock);
  int r = sendto(socket.sock, msg, msg_size, 0, (struct sockaddr *)&socket.server, socket.addr_len);
  if( r != msg_size )
    error("Client send");
  return r;
}


int IDSIA::ServerReceiveDataFromSocketNonBlocking(Socket &socket, char **msg, int &msg_size, const int max_data)
{
  //fprintf(stderr, "Server On receive max %d bytes from socket %d\n", max_data, socket.sock);

  msg_size = recvfrom(socket.sock, *msg, max_data, MSG_DONTWAIT, (struct sockaddr *)&socket.client, (socklen_t *)&socket.addr_len);

  if (msg_size <= 0)
    msg_size = 0;
  else
    {
      //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);
    }

  return msg_size;
}

int IDSIA::ServerReceiveDataFromSocketNonBlocking(Socket &socket, char *msg, int &msg_size, const int max_data)
{
  //fprintf(stderr, "Server On receive max %d bytes from socket %d\n", max_data, socket.sock);

  msg_size = recvfrom(socket.sock, msg, max_data, MSG_DONTWAIT, (struct sockaddr *)&socket.client, (socklen_t *)&socket.addr_len);

  if (msg_size <= 0)
    msg_size = 0;
  else
    {
      //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);
    }

  return msg_size;
}


int IDSIA::ServerPeekDataFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int max_data)
{
  //fprintf(stderr, "Server peek max %d bytes from socket %d\n", max_data, socket.sock);

  msg_size = recvfrom(socket.sock, msg, max_data, MSG_PEEK, (struct sockaddr *)&socket.client, (socklen_t *)&socket.addr_len);

  if (msg_size < 0)
    error("Server Recvfrom peek");

  //fprintf(stderr, "[Peek] Received %d bytes from socket %d\n", msg_size, socket.sock);

  return msg_size;
}


int IDSIA::ServerPeekDataFromSocketNonBlocking(Socket &socket, char *msg, int &msg_size, const int max_data)
{
  //fprintf(stderr, "Server peek max %d bytes from socket %d\n", max_data, socket.sock);

  msg_size = recvfrom(socket.sock, msg, max_data, MSG_PEEK | MSG_DONTWAIT, (struct sockaddr *)&socket.client, (socklen_t *)&socket.addr_len);

  if (msg_size <= 0)
    msg_size = 0;
  else
    {
      //fprintf(stderr, "[Peek] Received %d bytes from socket %d\n", msg_size, socket.sock);
    }

  return msg_size;
}



int IDSIA::ServerReceiveFixedDataLenFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int data_len)
{
  //fprintf(stderr, "Server On receive %d bytes from socket %d\n", data_len, socket.sock);

  msg_size = recvfrom(socket.sock, msg, data_len, MSG_WAITALL, (struct sockaddr *)&socket.client, (socklen_t *)&socket.addr_len);

  if (msg_size < 0)
    error("Server Recvfrom fixed");

  //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);

  return msg_size;
}


int IDSIA::ServerReceiveDataFromSocketBlocking(Socket &socket, char **msg, int &msg_size, const int max_data)
{
  //fprintf(stderr, "Server On receive max %d bytes from socket %d\n", max_data, socket.sock);

  msg_size = recvfrom(socket.sock, *msg, max_data, 0, (struct sockaddr *)&socket.client, (socklen_t *)&socket.addr_len);

  if (msg_size < 0) 
    error("Server Recvfrom");

  //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);
  return msg_size;
}


int IDSIA::ServerReceiveDataFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int max_data)
{

  //fprintf(stderr, "On receive max %d bytes from socket %d\n", max_data, socket.sock);


  msg_size = recvfrom(socket.sock, msg, max_data, 0, (struct sockaddr *)&socket.client, (socklen_t *)&socket.addr_len);

  if (msg_size < 0) 
    error("Server Recvfrom");

  //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);
  return msg_size;
}


int IDSIA::ClientReceiveDataFromSocketNonBlocking(Socket &socket, char **msg, int &msg_size, const int max_data)
{

  //fprintf(stderr, "Client On receive max %d bytes from socket %d\n", max_data, socket.sock);

  struct sockaddr from;

  msg_size = recvfrom(socket.sock, *msg, max_data, MSG_DONTWAIT, (struct sockaddr *)&from, (socklen_t *)&socket.addr_len);

  if (msg_size <= 0)
    msg_size = 0;
  else
    {
      //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);
    }

  return msg_size;
}


int IDSIA::ClientReceiveDataFromSocketNonBlocking(Socket &socket, char *msg, int &msg_size, const int max_data)
{

  //fprintf(stderr, "Client On receive max %d bytes from socket %d\n", max_data, socket.sock);

  struct sockaddr from;

  msg_size = recvfrom(socket.sock, msg, max_data, MSG_DONTWAIT, (struct sockaddr *)&from, (socklen_t *)&socket.addr_len);

  if (msg_size <= 0)
    msg_size = 0;
  else
    {
      //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);
    }

  return msg_size;
}


int IDSIA::ClientReceiveFixedDataLenFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int data_len)
{
  //fprintf(stderr, "Client On receive %d bytes from socket %d\n", data_len, socket.sock);

  struct sockaddr from;

  msg_size = recvfrom(socket.sock, msg, data_len, MSG_WAITALL, (struct sockaddr *)&from, (socklen_t *)&socket.addr_len);

  if (msg_size < 0)
    error("Client Recvfrom fixed");

  //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);

  return msg_size;
}


int IDSIA::ClientReceiveDataFromSocketBlocking(Socket &socket, char **msg, int &msg_size, const int max_data)
{
  //fprintf(stderr, "Client On receive max %d bytes from socket %d\n", max_data, socket.sock);

  struct sockaddr from;

  msg_size = recvfrom(socket.sock, *msg, max_data, 0, (struct sockaddr *)&from, (socklen_t *)&socket.addr_len);

  if (msg_size < 0) 
    error("Client Recvfrom");

  //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);
  return msg_size;
}


int IDSIA::ClientReceiveDataFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int max_data)
{

  //fprintf(stderr, "Client On receive max %d bytes from socket %d\n", max_data, socket.sock);

  struct sockaddr from;

  msg_size = recvfrom(socket.sock, msg, max_data, 0, (struct sockaddr *)&from, (socklen_t *)&socket.addr_len);

  if (msg_size < 0) 
    error("Client Recvfrom");

  //fprintf(stderr, "Received %d bytes from socket %d\n", msg_size, socket.sock);
  return msg_size;
}


int IDSIA::ClientPeekDataFromSocketBlocking(Socket &socket, char *msg, int &msg_size, const int max_data)
{

  //fprintf(stderr, "Client peek max %d bytes from socket %d\n", max_data, socket.sock);

  struct sockaddr from;

  msg_size = recvfrom(socket.sock, msg, max_data, MSG_PEEK, (struct sockaddr *)&from, (socklen_t *)&socket.addr_len);

  if (msg_size < 0) 
    error("Client Recvfrom");

  //fprintf(stderr, "[Peek] Received %d bytes from socket %d\n", msg_size, socket.sock);
  return msg_size;
}

/***********************************************************************************/
/***  These functions are only used from the networks simulator side   ***/
/*                                                                    */
/***********************************************************************************/

void IDSIA::BuildNetworkSimInformationPacket(char **msg, int &msg_size, const char *sim_name)
{
  NetworkSimInfoPacket info;

  info.type            = NETWORK_SIM_INFORMATION;
  strcpy(info.network_sim, sim_name);

  memcpy(*msg, (void *)&info, sizeof(NetworkSimInfoPacket));

  msg_size = sizeof(NetworkSimInfoPacket);
}


void IDSIA::BuildNetworkSimInformationPacket(char *msg, int &msg_size, const char *sim_name)
{
  NetworkSimInfoPacket info;

  info.type            = NETWORK_SIM_INFORMATION;
  strcpy(info.network_sim, sim_name);

  memcpy(msg, (void *)&info, sizeof(NetworkSimInfoPacket));

  msg_size = sizeof(NetworkSimInfoPacket);
}



void IDSIA::GenerateRandomMsg(char **msg, int &msg_size, const int from, const int robot_num, const Real timestamp)
{
  float rnd = ((double)rand() / RAND_MAX) * robot_num;
  if( rnd > (0.25 * robot_num) )
    {
      char payload[256];
      ApplicationPacket appl;
      appl.type = APPLICATION_DATA;
      appl.from = from;
      appl.to   = (int)(((double)rand() / RAND_MAX) * robot_num);
      appl.timestamp = timestamp;
      sprintf(payload, "Message from %d to %d, sent at time %g", appl.from, appl.to, timestamp);
      appl.size = strlen(payload) + 1;

      memcpy(*msg, (void *)&appl, sizeof(ApplicationPacket));
      memcpy((*msg) + sizeof(ApplicationPacket), (void *)&payload, appl.size);

      msg_size = sizeof(ApplicationPacket) + appl.size;
    }
  else
    msg_size = 0;
}


void IDSIA::GenerateRandomMsg(char *msg, int &msg_size, const int from, const int robot_num, const Real timestamp)
{
  float rnd = ((double)rand() / RAND_MAX) * robot_num;
  if( rnd > (0.25 * robot_num) )
    {
      char payload[256];
      ApplicationPacket appl;
      appl.type = APPLICATION_DATA;
      appl.from = from;
      appl.to   = (int)(((double)rand() / RAND_MAX) * robot_num);
      sprintf(payload, "Message from %d to %d, sent at time %g", appl.from, appl.to, timestamp);
      appl.size = strlen(payload) + 1;

      memcpy(msg, (void *)&appl, sizeof(ApplicationPacket));
      memcpy((msg) + sizeof(ApplicationPacket), (void *)&payload, appl.size);

      msg_size = sizeof(ApplicationPacket) + appl.size;
    }
  else
    msg_size = 0;
}

void IDSIA::GenerateMsg(char **msg, int &msg_size, const int from, const int to, const string content, const Real timestamp)
{
   ApplicationPacket appl;
   appl.type = APPLICATION_DATA;
   appl.from = from;
   appl.to   = to;
   appl.timestamp = timestamp;
   appl.size = content.size()+1;

   memcpy(*msg, (void *)&appl, sizeof(ApplicationPacket));
   memcpy((*msg) + sizeof(ApplicationPacket), (void *)content.c_str(), appl.size);

   msg_size = sizeof(ApplicationPacket) + appl.size;
}

/***********************************************************************************/

/***********************************************************************************/
/***  These functions are only used from the robot simulator side   ***/
/*                                                                    */
/***********************************************************************************/

void IDSIA::BuildSimulationInformationPacket(char **msg, int &msg_size, const int robot_num, 
					     const Real simulation_step, const Real simulation_time, 
					     const char *sim_name, const Real stop_time)
{
  SimulationInformationPacket info;

  info.type            = SIMULATION_INFORMATION;
  info.robot_num       = robot_num;
  info.simulation_step = simulation_step;
  info.start_time      = simulation_time;
  info.stop_time       = stop_time;
  strcpy(info.simulator_name, sim_name);

  memcpy(*msg, (void *)&info, sizeof(SimulationInformationPacket));

  msg_size = sizeof(SimulationInformationPacket);
}

void IDSIA::BuildSimulationInformationPacket(char *msg, int &msg_size, const int robot_num, 
					     const Real simulation_step, const Real simulation_time, 
					     const char *sim_name, const Real stop_time)
{
  SimulationInformationPacket info;

  info.type            = SIMULATION_INFORMATION;
  info.robot_num       = robot_num;
  info.simulation_step = simulation_step;
  info.start_time      = simulation_time;  
  info.stop_time       = stop_time;
  strcpy(info.simulator_name, sim_name);

  memcpy(msg, (void *)&info, sizeof(SimulationInformationPacket));

  msg_size = sizeof(SimulationInformationPacket);
}


void IDSIA::BuildRobotStartPacket(char **msg, int &msg_size, const int id, const char *robot_type)
{
  RobotStartPacket start;

  start.type  = START_FROM_ROBOT_PROCESS;
  start.id    = id;
  strcpy(start.robot_type, robot_type);

  memcpy(*msg, (void *)&start, sizeof(RobotStartPacket));

  msg_size = sizeof(RobotStartPacket);
}

void IDSIA::BuildRobotStartPacket(char *msg, int &msg_size, const int id, const char *robot_type)
{
  RobotStartPacket start;

  start.type  = START_FROM_ROBOT_PROCESS;
  start.id    = id;
  strcpy(start.robot_type, robot_type);

  memcpy(msg, (void *)&start, sizeof(RobotStartPacket));

  msg_size = sizeof(RobotStartPacket);
}


void IDSIA::BuildRobotPositionsPacket(char **msg, int &msg_size, const int robot_num, const Position *positions, const Real timestamp)
{
  NodePositionsPacket pos;

  pos.type = NODE_POSITIONS;
  pos.timestamp = timestamp;
  pos.robot_num = robot_num;
  pos.size = robot_num * sizeof(Position);
  
  memcpy(*msg, (void *)&pos, sizeof(NodePositionsPacket));
  memcpy((*msg) + sizeof(NodePositionsPacket), (void *)positions, pos.size);
  
  msg_size = sizeof(NodePositionsPacket) + pos.size;
}


void IDSIA::BuildRobotPositionsPacket(char *msg, int &msg_size, const int robot_num, const Position *positions, const Real timestamp)
{
  NodePositionsPacket pos;

  pos.type = NODE_POSITIONS;
  pos.timestamp = timestamp;
  pos.robot_num = robot_num;
  pos.size = robot_num * sizeof(Position);
  
  memcpy(msg, (void *)&pos, sizeof(NodePositionsPacket));
  memcpy((msg) + sizeof(NodePositionsPacket), (void *)positions, pos.size);
  
  msg_size = sizeof(NodePositionsPacket) + pos.size;
}


void IDSIA::GenerateRandomPositions(Position *positions, const int nodes_num, const Real xmax, const Real ymax, const Real zmax)
{
  for(int i=0; i < nodes_num; i++)
    {
      positions[i].id = i;
      positions[i].x = ((Real)rand() / RAND_MAX) * xmax;
      positions[i].y = ((Real)rand() / RAND_MAX) * ymax;
      positions[i].z = ((Real)rand() / RAND_MAX) * zmax;
    }

  for(int i=0; i < nodes_num; i++)
    fprintf(stderr, "[%d] pos[%d] (%g %g %g)\n", i, positions[i].id, positions[i].x, positions[i].y, positions[i].z);

}
