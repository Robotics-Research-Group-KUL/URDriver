#ifndef OROCOS_URDriverRT_dummy_server_COMPONENT_HPP
#define OROCOS_URDriverRT_dummy_server_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include "RTDeserialize.hpp"

#pragma pack(1)
	struct data_struct{
		int Message_Size;
		double Time;
		double q_target[6],
		qd_target	[6],
		qdd_target	[6],
		I_target	[6],
		M_target	[6],
		q_actual	[6],
		qd_actual	[6],
		I_actual	[6],
		I_control	[6],
		Tool_vector_actual	[6],
		TCP_speed_actual	[6],
		TCP_force	[6],
		Tool_vector_target	[6],
		TCP_speed_target[6]	;
		double Digital_input_bits;
		double Motor_temperatures[6];
		double Controller_Timer		,
		Test_value	,
		Robot_Mode	;
		double  Joint_Modes[6];
		double Safety_Mode	;
		double UNUSED1[6]	;//6
		double Tool_Accelerometer_values[3];//3
		double UNUSED2	[6];//6
		double Speed_scaling,
		Linear_momentum_norm,
		UNUSED3,
		UNUSED4,
		V_main	,
		V_robot,
		I_robot;
		double V_actual[6];//6
	} ;
using namespace std;
/** Component for programming the robot and interface with the custom data exchanged with the robot.
 * it can send the program to the robot,
 * it open a server to which the robot connects
 */
class URDriverRT_dummy_server : public RTT::TaskContext{
public:
	URDriverRT_dummy_server(std::string const& name);
	bool configureHook();
	bool startHook();
	void updateHook();


	bool open_server();

private:
	//this function modifies the data in vec!
	bool send_out(int vec[],const unsigned int );

	bool sending_velocity;
	//!@name Properties
	///@{

	int reverse_port_number;
	string prop_adress;

	///@}
	fd_set         sock;

	string buffer;

	int sockfd, listenfd, newsockfd;

	struct sockaddr_in robot_addr;
	struct sockaddr_in program_server_addr;
	struct sockaddr_in cli_addr;

	//struct hostent *server;

	data_struct data_frame;
	double time_now;



};

inline int make_socket (uint16_t port)
{
	int sock;
	struct sockaddr_in name;

	/* Create the socket. */
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		return -1;
	}

	/* Give the socket a name. */
	name.sin_family = AF_INET;
	name.sin_port = htons (port);
	name.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0)
	{
		return -2;
	}

	return sock;
}
#endif
