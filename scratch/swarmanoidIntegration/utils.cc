#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <list>
#include "utils.h"

#define DEBUG 0

using namespace idsia_utils;

position idsia_utils::parseLine(std::string line){
  /*Line format
    int double double double
  */
  std::istringstream iss(line);
  position p;
  iss >> p.node;
  iss >> p.x;
  iss >> p.y;
  iss >> p.z;

  #if DEBUG
  /*DEBUG*/
  std::cout << "----- node->" << p.node << " x: " << p.x << " y: " << p.y << " z: " << p.z << std::endl;
  #endif
  return p;
}

std::vector<position> idsia_utils::readFile(std::string fileName){
  
  std::fstream f;
  f.open(fileName.c_str(), std::fstream::in);
  char buffer[50];
  f.getline(buffer,50);
  
  std::string s;
  int numNodes;

  std::istringstream iss(buffer);
  iss >> s >> numNodes;
  
  #if DEBUG
  /*DEBUG*/
  std::cout << "String: " << s << " # nodes: " << numNodes << std::endl;
  #endif
  std::vector<position> posList(numNodes);
  
  for(int i=0; i<numNodes;i++){
    f.getline(buffer,50);
    posList[i] = parseLine(std::string(buffer));
  }

  
  #if DEBUG
  
  std::vector<position>::iterator it;
  for(it=posList.begin();it!=posList.end();it++){
    std::cout << "node->" << it->node << " x: " << it->x << " y: " << it->y << " z: " << it->z << std::endl;
  }
  #endif

  return posList;
}

app idsia_utils::parseAppLine(std::string line){
  
 /*Line format
    std::string int int double double double
  */
  std::istringstream iss(line);
  app a;
  iss >> a.type >> a.sender >> a.remote >> a.interval >> a.start >> a.end;
  return a;
}

std::list<app>* idsia_utils::readAppFile(std::string fileName){
  
  std::ifstream f;
  f.open(fileName.c_str(), std::fstream::in);
  char buffer[200];


  std::string s;
  int apps;

  f.getline(buffer,50);
  std::istringstream iss(buffer);
  iss >> s >> apps;
  std::list<app> *appList = new std::list<app>();
  app a;
  /*
  while(!f.eof()){
    std::cerr << i++ << std::endl;
    f.getline(buffer,200,'\n');
    a = parseAppLine(std::string(buffer));
    appList->push_back( a);
  }
  */
  for(int i=0; i<apps;i++){
    f.getline(buffer,200);
    a = parseAppLine(std::string(buffer));
    appList->push_back( a);
  }

  return appList;
}


/*
int main(){
  std::list<app>* l= readAppFile("apps.conf");
  std::cerr << "List size: " << l->size() << std::endl;
  std::list<app>::iterator it;
  for(it=l->begin(); it!=l->end();it++){
    app a = *it;
     std::cerr << " Control -->  Type: " << a.type << " Sender: " <<  a.sender << " Remote: " << a.remote <<  " Interval " << a.interval << " Start: " <<  a.start << " end: " <<  a.end << std::endl;
  }
  return 0;
}
*/
