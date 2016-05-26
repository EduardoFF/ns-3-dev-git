#ifndef _NS3_NETWORK_APPLICATION_HELPER_H_
#define _NS3_NETWORK_APPLICATION_HELPER_H_

#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/ipv4-interface-container.h"
using namespace ns3;

namespace IDSIA{
  class NetworkApplicationHelper{
  public:
    NetworkApplicationHelper(uint16_t portBase, uint16_t numberOfRobots, double simulationStep, uint16_t ns3Port, Ipv4InterfaceContainer inter);
    void SetAttribute (std::string name, const AttributeValue &value);
    ApplicationContainer Install (NodeContainer c) const;
    ApplicationContainer Install (Ptr<Node> node) const;
    ApplicationContainer Install (std::string nodeName) const;

  private:
    Ptr<Application> InstallPriv (Ptr<Node> node) const;
    Ipv4InterfaceContainer iface;
    int m_portBase;
    int numberOfRobots;
    ObjectFactory m_factory;
  };

}//end namespace IDSIA
#endif /* _NS3_NETWORK_APPLICATION_HELPER_H_ */
