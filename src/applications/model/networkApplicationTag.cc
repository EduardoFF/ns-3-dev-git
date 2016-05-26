#include "networkApplicationTag.h"

#include "ns3/simulator.h"

using namespace ns3;

namespace IDSIA{
  uint32_t NetworkApplicationTag::ctr = 0;

  TypeId NetworkApplicationTag::GetTypeId(void){
    static TypeId tid = TypeId("IDSIA::NetworkApplicationTag")
      .SetParent<Tag>()
      .AddConstructor<NetworkApplicationTag>();

    return tid;
  }

  TypeId NetworkApplicationTag::GetInstanceTypeId(void) const{
    return GetTypeId();
  }

  NetworkApplicationTag::NetworkApplicationTag(){
    this->id = ctr++;
    this->creationTime = Simulator::Now();
  }

  void NetworkApplicationTag::SetPhySendingTime(Time t){
    this->phySendingTime = t;
  }

  void NetworkApplicationTag::SetSender(uint32_t s){
    this->sender = s;
  }

  Time NetworkApplicationTag::GetPhySendingTime(void){
    return this->phySendingTime;
  }

  Time NetworkApplicationTag::GetCreationTime(void){
    return this->creationTime;
  }

  uint32_t NetworkApplicationTag::GetId(void){
    return this->id;
  }

  uint32_t NetworkApplicationTag::GetSerializedSize(void) const{
    uint32_t s = 0;
    //adding the size of the id + sender
    s+= sizeof(uint32_t)*2;
    //adding the size of the 2 Time value
    s+= sizeof(uint64_t)*2;
    return s;
  }
  uint32_t NetworkApplicationTag::GetSender(void){ 
    return this->sender; 
  }
  void NetworkApplicationTag::Serialize(TagBuffer i) const{
    i.WriteU32(id);
    i.WriteU32(sender);
    uint64_t us = phySendingTime.GetMicroSeconds();
    i.WriteU64(us);
    us = creationTime.GetMicroSeconds();
    i.WriteU64(us);
  }

  void NetworkApplicationTag::Deserialize(TagBuffer i){
    id = i.ReadU32();
    sender = i.ReadU32();
    phySendingTime = Time(MicroSeconds(i.ReadU64()));
    creationTime = Time(MicroSeconds(i.ReadU64()));
  }
  void NetworkApplicationTag::Print (std::ostream &os) const{
    
  }
}
