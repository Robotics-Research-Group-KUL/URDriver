#ifndef OROCOS_URDriver_program_COMPONENT_HPP
#define OROCOS_URDriver_program_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include "URDriver/Deserialize.hpp"
#include "URDriver/utils.hpp"

#define MSG_OUT			1
#define MSG_QUIT		2
#define MSG_JOINT_STATES	3
#define MSG_MOVEJ		4
#define MSG_WAYPOINT_FINISHED	5
#define MSG_STOPJ		6
#define MSG_SERVOJ		7
#define MSG_SET_PAYLOAD		8
#define MSG_WRENCH		9
#define MSG_SET_DIGITAL_OUT	10
#define MSG_GET_IO		11
#define MSG_SET_FLAG		12
#define MSG_SET_TOOL_VOLTAGE	13
#define MSG_SET_ANALOG_OUT	14
#define MULT_wrench		10000.0
#define MULT_payload		1000.0
#define MULT_jointstate		10000.0
#define MULT_time		1000000.0
#define MULT_blend		1000.0
#define MULT_analog		1000000.0

#define MSG_VELJ		20
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
	bool send_reset_program();
	bool send_joint_objective(vector<double>q, double time);
	bool open_server();
	bool start_send_velocity();
	bool stop_send_velocity();
private:
	//this function modifies the data in vec!
	bool send_out(int vec[],const unsigned int );

	bool sending_velocity;
	//!@name Properties
	///@{
	int port_number;
	int reverse_port_number;
	string prop_adress;//<Address of robot
	string my_adress;//<Address of pc where component is runnung
	string program_file;//<Address of pc where component

	double velocity_apl;//TODO take this out
	double acc_limit;
	double freq;//TODO take this out
	double timeStepMultiplier;
	///@}
	fd_set         sock;

	string buffer;
	bool ready_to_send_program;
	bool program_sent;
	bool server_ok;
	int sockfd, listenfd, newsockfd;

	struct sockaddr_in robot_addr;
	struct sockaddr_in program_server_addr;
	struct sockaddr_in cli_addr;

	//struct hostent *server;


double time_now;

	//!@name Ports
	///@{
	RTT::InputPort<vector<double> > qdes_inport;
	vector<double>  qdes;
	//RTT::OutputPort<vector<double> > q_qctual_outport;
	//RTT::OutputPort<double > time_outport;
	///@}

};


#endif
