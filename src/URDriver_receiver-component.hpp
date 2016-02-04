#ifndef OROCOS_URDriver_COMPONENT_HPP
#define OROCOS_URDriver_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include "Deserialize.hpp"

using namespace std;

class URDriver_receiver : public RTT::TaskContext{
  public:
    URDriver_receiver(std::string const& name);
    bool configureHook();
    bool startHook();
    void updateHook();
    void stopHook();
    void cleanupHook();

  private:

    //!@name Properties
    ///@{
    int port_number;
    string prop_adress;
    ///@}


	int sockfd;

	struct sockaddr_in serv_addr;
	struct hostent *server;


	URdata::Ptr data_pointer;

	vector<double> v6; ///<internal vector of length 6
	//!@name Ports
	///@{
RTT::OutputPort<int > bytes_outport;
  RTT::OutputPort<bool > isProgramRunning;
  RTT::OutputPort<bool > isProgramPaused;
	//RTT::OutputPort<vector<double> > q_qctual_outport;
	//RTT::OutputPort<double > time_outport;
	///@}

};

#endif
