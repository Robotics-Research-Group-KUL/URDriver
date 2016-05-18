#ifndef OROCOS_URDriverRT_COMPONENT_HPP
#define OROCOS_URDriverRT_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include "URDriver/RTDeserialize.hpp"
#include <rtt/extras/FileDescriptorActivity.hpp>

using namespace std;

class URDriverRT_receiver : public RTT::TaskContext{
  public:
    URDriverRT_receiver(std::string const& name);
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

	RTT::extras::FileDescriptorActivity* act;
	int sockfd;

	struct sockaddr_in serv_addr;
	struct hostent *server;


	RTdata::Ptr data_pointer;

	vector<double> v6; ///<internal vector of length 6
	//!@name Ports
	///@{
	RTT::OutputPort<vector<double> > q_actual_outport;
	RTT::OutputPort<vector<double> > qd_actual_outport;
	RTT::OutputPort<double > time_outport;
	///@}

};

#endif
