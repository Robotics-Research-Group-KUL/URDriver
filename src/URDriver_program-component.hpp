#ifndef OROCOS_URDriver_program_COMPONENT_HPP
#define OROCOS_URDriver_program_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include "Deserialize.hpp"

using namespace std;
/** Component for programming the robot and interface with the custom data exchanged with the robot.
 * it can send the program to the robot,
 * it open a server to which the robot connects
 */
class URDriver_program : public RTT::TaskContext{
  public:
    URDriver_program(std::string const& name);
    bool configureHook();
    bool startHook();
    void updateHook();
    void stopHook();
    void cleanupHook();
    bool send_program();
    bool  send_reset_program();
  private:

    //!@name Properties
    ///@{
    int port_number;
    int reverse_port_number;
    string prop_adress;
    ///@}


bool ready_to_send_program;

	int sockfd, listenfd;

	struct sockaddr_in robot_addr;
	struct sockaddr_in program_server_addr;
	//struct hostent *server;




	//!@name Ports
	///@{
	//RTT::OutputPort<vector<double> > q_qctual_outport;
	//RTT::OutputPort<double > time_outport;
	///@}

};

#endif
