#include "ns3-network-application-helper.h"

#include "ns3/ns3-network-application.h"
#include "ns3/string.h"
#include "ns3/names.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"


using namespace ns3;

namespace IDSIA{

  NetworkApplicationHelper::NetworkApplicationHelper(uint16_t portBase, uint16_t numberOfRobots, double simulationStep, uint16_t ns3Port, Ipv4InterfaceContainer inter){
    m_factory.SetTypeId("IDSIA::NetworkApplication");
    m_factory.Set("portBase", UintegerValue(portBase));
    m_factory.Set("numberOfRobots",UintegerValue(numberOfRobots));
    m_factory.Set("simulationStep", DoubleValue(simulationStep));
    m_factory.Set("port", UintegerValue(ns3Port)); 
    this -> iface = inter;
  }

  void NetworkApplicationHelper::SetAttribute(std::string name, const AttributeValue &value){
    m_factory.Set(name, value);
  }

  ApplicationContainer NetworkApplicationHelper::Install(Ptr<Node> node) const{
    return ApplicationContainer(InstallPriv(node));
  }

  ApplicationContainer NetworkApplicationHelper::Install(std::string nodeName) const{
    Ptr<Node> node = Names::Find<Node>(nodeName);
    return ApplicationContainer(InstallPriv(node));
  }

  ApplicationContainer NetworkApplicationHelper::Install(NodeContainer c) const{
    ApplicationContainer apps;
    for(NodeContainer::Iterator i = c.Begin();i!=c.End();i++){
      apps.Add(InstallPriv(*i));
    }
    return apps;
  }

  Ptr<Application> NetworkApplicationHelper::InstallPriv(Ptr<Node> node) const{
    Ptr<Application> app = m_factory.Create<Application>();
    Ptr<NetworkApplication> ana = DynamicCast<NetworkApplication>(app); 
    ana->SetInterfaceContainer(this->iface);
    node->AddApplication(ana);
 
    return app;
  }

}//end namespace IDSIA
