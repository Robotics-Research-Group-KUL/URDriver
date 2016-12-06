#ifndef OROCOS_URDriverRT_COMPONENT_HPP
#define OROCOS_URDriverRT_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include "URDriver/RTDeserialize.hpp"
#include <rtt/extras/FileDescriptorActivity.hpp>
#include <rtt/os/TimeService.hpp>
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
    string prop_address;
    ///@}

	RTT::extras::FileDescriptorActivity* act;
	int sockfd;
	RTT::os::TimeService::ticks                     m_time_begin;
	RTT::os::TimeService::Seconds                   m_time_passed;

	struct sockaddr_in serv_addr;
	struct hostent *server;


	RTdata::Ptr data_pointer;

	vector<double> v6; ///<internal vector of length 6
	//!@name Ports
	///@{
	RTT::OutputPort<vector<double> > q_actual_outport;
	RTT::OutputPort<vector<double> > qd_actual_outport;
	RTT::OutputPort<double > time_outport;
	RTT::OutputPort<double > period_outport;
	///@}

};

#endif
