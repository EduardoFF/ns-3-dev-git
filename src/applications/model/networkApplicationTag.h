#ifndef _NETWORKAPPLICATIONTAG_H_
#define _NETWORKAPPLICATIONTAG_H_

#include "ns3/tag.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/nstime.h"


using namespace ns3;

namespace IDSIA{
  class NetworkApplicationTag : public Tag{
  public:
    static TypeId GetTypeId (void); //required by ns3
    virtual TypeId GetInstanceTypeId(void) const;
    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(TagBuffer i) const;
    virtual void Deserialize(TagBuffer i);
    NetworkApplicationTag();
    virtual void Print (std::ostream &os) const;
    //accessor 
    void SetPhySendingTime(Time t);
    Time GetPhySendingTime(void);
    Time GetCreationTime(void);
    uint32_t GetId(void);
    uint32_t GetSender(void);
    void SetSender(uint32_t);

  private:
    Time phySendingTime;
    Time creationTime;
    uint32_t id;
    uint32_t sender;
    static uint32_t ctr;
  };
  
}//end namespace IDSIA

#endif /* _NETWORKAPPLICATIONTAG_H_ */
