#include "URDriver_program-component.hpp"
#include <rtt/Component.hpp>
#include <iostream>
#include <fstream>      // std::ifstream
#include "utils.hpp"      // std::ifstream
#include <libexplain/bind.h>
#include <signal.h>
using namespace RTT;
URDriver_program::URDriver_program(std::string const& name) : TaskContext(name,PreOperational)
      , prop_adress("192.168.1.102")
      , port_number(30002)
      , ready_to_send_program(false)
      , reverse_port_number(50001)
      , qdes(6,0.0)
      , velocity_apl(0.6)
      , acc_limit(4)
      , freq(1.0)
{
	addProperty("port_number",port_number);
	addProperty("reverse_port_number",reverse_port_number);
	addProperty("prop_adress",prop_adress).doc("ip address robot");

	addOperation("send_reset_program", &URDriver_program::send_reset_program, this, RTT::OwnThread);
	addOperation("send_program", &URDriver_program::send_program, this, RTT::OwnThread);
	addOperation("send_joint_objective",
		     &URDriver_program::send_joint_objective, this, RTT::OwnThread);
	addOperation("open_server",
		     &URDriver_program::open_server, this, RTT::OwnThread);

	addOperation("start_send_velocity",
		     &URDriver_program::start_send_velocity, this, RTT::OwnThread);
	addOperation("stop_send_velocity",
		     &URDriver_program::stop_send_velocity, this, RTT::OwnThread);


	addEventPort("qdes_inport",qdes_inport);

	/* */


	//test stuff
	addProperty("velocity_apl",velocity_apl);
	addProperty("acc_limit",acc_limit);
	addProperty("freq",freq);


	buffer.reserve(1024);
	server_ok=false;
	sending_velocity=false;



}

bool URDriver_program::start_send_velocity(){
	sending_velocity=true;
}
bool URDriver_program::stop_send_velocity(){
	sending_velocity=false;
}

void signal_callback_handler(int signum){

	printf("Caught signal SIGPIPE %d\n",signum);
}
bool URDriver_program::configureHook(){

	ready_to_send_program=false;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<": not able to open the socket..." << endlog();
		return false;
	}

	bzero((char *) &robot_addr, sizeof(robot_addr));
	robot_addr.sin_family = AF_INET;
	robot_addr.sin_port=htons(port_number);
	//Convert from presentation format to an Internet number
	if(inet_pton(AF_INET, prop_adress.c_str(), &robot_addr.sin_addr)<=0)
	{
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<":the string "<<prop_adress
			 <<" is not a good formatted string for address ( like 127.0.0.1)"
			<< endlog();
		return false;
	}

	cout<<"connect to robot port"<<endl;
	Logger::In in(this->getName());
	if( connect(sockfd, (struct sockaddr *)&robot_addr, sizeof(robot_addr)) < 0)
	{
		log(Error)<<this->getName()<<": Connection failed!"<< endlog();
		return false;
	}
	log(Info)<<this->getName()<<": Connection OK!"<< endlog();



	//configure server to receive data from the program/robot

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	int yes=1;
	//char yes='1'; // use this under Solaris


	//TODO take these lines out...
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		cout<<"error setsockopt"<<endl;
		return false;
	}
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1) {
		cout<<"error setsockopt 2"<<endl;
		return false;
	}


	bzero((char *) &program_server_addr, sizeof(program_server_addr));
	program_server_addr.sin_family = AF_INET;
	program_server_addr.sin_addr.s_addr = INADDR_ANY;
	program_server_addr.sin_port = htons(50001);//TODO




	int bind_ret=bind(listenfd, (struct sockaddr*)&program_server_addr, sizeof(program_server_addr));
	if(bind_ret< 0)
	{
		Logger::In in(this->getName());
		char message[3000];
		explain_message_bind(message, sizeof(message),
				     listenfd, (struct sockaddr*)&program_server_addr, sizeof(program_server_addr));
		log(Error)<<this->getName()<<": error binding socket server.\n"
			 <<"bind ret: "<<bind_ret<<
			   "\nerrno: "<<errno<<
			   "\nMessage:"<<message<<endlog();

		return false;
	}


	return true;
}
bool URDriver_program::open_server()
{

	int listen_ret=listen(listenfd, 1);
	if(listen_ret!=0) //maybe 1 is ok
	{
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<": error listening socket server.\n"
			 <<"listen_ret "<<listen_ret<< endlog();

		return false;
	}
	cout<<"after listen"<<endl;
	socklen_t  clilen = sizeof(cli_addr);
	newsockfd = accept(listenfd,
			   (struct sockaddr *) &cli_addr,
			   &clilen);
	cout<<"after accept"<<endl;
	if (newsockfd < 0)
	{
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<": error accepting connection.\n"
			 <<"listen_ret "<<listen_ret<< endlog();

		return false;
	}

	FD_ZERO(&sock);
	FD_SET(newsockfd,&sock);
	server_ok=true;
	return true;
}

bool URDriver_program::startHook(){


	time_now=0;
	qdes[4]=0;
	return true;
	//return server_ok;
}





void URDriver_program::updateHook(){

	signal(SIGPIPE, signal_callback_handler);
	if(sending_velocity)
	{

		/*if(qdes_inport.read(qdes)!=NoData)
		{
			if (qdes.size()!=6)
				Logger::In in(this->getName());
			log(Error)<<this->getName()<<": error size of q in port "<<qdes_inport.getName()<<".\n STOPPING."<< endlog();
			this->stop();


		}*/

		int data_frame[9];
		double time=getPeriod()*1;//make the function on robot side returns before he get new data
		time_now+=getPeriod();
		qdes[4]=sin(time_now*3.14*freq)*velocity_apl;
		data_frame[0]=MSG_VELJ;
		for (int i=0;i<6;i++)
			data_frame[1+i]=(int)(qdes[i]*MULT_jointstate);
		data_frame[7]=(int)(acc_limit*MULT_jointstate);//max acc
		data_frame[8]=(int)(time*MULT_time);//time
		if (!send_out(data_frame,9))
		{
			Logger::In in(this->getName());
			log(Error)<<this->getName()<<": error send_joint_velocity. STOPPING."<< endlog();
			this->stop();
			return;
		}

	}

	struct timeval timeout = {0, 0};   // polling

	fd_set read_fd_set = sock;

	int retval = select(newsockfd+1, &read_fd_set, NULL, NULL, &timeout);
	if (retval <= 0)
	{
		//cout<<"retval "<<retval<<endl;
		return;
	}
	else// the socket has data
	{


		int msg_type;
		int n = read(newsockfd,& msg_type, sizeof(msg_type));
		swap(msg_type);
		if  (n <= 0)
		{
		/*	Logger::In in(this->getName());
			log(Error)<<this->getName()<<": error in read, stopping."<< endlog();
			this->stop();*/
			return;
		}
		switch(msg_type){
		case MSG_WAYPOINT_FINISHED:
			cout<<"MSG_WAYPOINT_FINISHED"<<endl;
			int way_point;
			n = read(newsockfd, &way_point, sizeof(way_point));
			cout<<"way_point: "<<way_point<<endl;
			break;
		case MSG_OUT:
			//cout<<"MSG_OUT"<<endl;
			buffer.clear();
			char c;
			int i;
			for(i=0;i<1024;i++)
			{
				n = read(newsockfd, &c, sizeof(c));
				if (c=='~')
					break;
			}
			//cout<<buffer<<endl;

			break;
		case MSG_QUIT:
			cout<<"MSG_QUIT"<<endl;
			break;
		default:
			cout<<"ERROR IN MSG_TYPE"<<endl;
		}
	}

}

bool URDriver_program::send_joint_objective(vector<double>q, double time){
	//if (!isRunning()) return false;
	if (q.size()!=6) return false;

	int data_frame[12];
	data_frame[0]=MSG_MOVEJ;
	data_frame[1]=101;//waypoint_id
	for (int i=0;i<6;i++)
		data_frame[2+i]=(int)(q[i]*MULT_jointstate);
	data_frame[8]=(int)(0.1*MULT_jointstate);//acc
	data_frame[9]=(int)(0.2*MULT_jointstate);//vel
	data_frame[10]=(int)(time*MULT_time);//time
	data_frame[11]=(int)(0.1*MULT_blend);//radius

	if (!send_out(data_frame,12))
	{
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<": error send_joint_objective."<< endlog();
		return false;
	}





	return true;
}
bool URDriver_program::send_out(int vec[], const unsigned int size){

	//swap all the vector in place
	for (unsigned int i=0;i<size;i++)
		swap(vec[i]);

	int n = write(newsockfd,vec,sizeof(int)*size);
	if (n < 0)
	{
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<": error writing socket."<< endlog();
		return false;
	}
	return true;

}


void URDriver_program::stopHook() {

}

void URDriver_program::cleanupHook() {
	close(listenfd);
	close(newsockfd);
	close(sockfd);
	//sigaction (SIGPIPE, &old_actn, NULL);
}
bool  URDriver_program::send_program(){
	//if (!ready_to_send_program) return false;
	std::ifstream t("prog.ur");
	std::stringstream buffer;
	buffer << t.rdbuf();
	string program=buffer.str();

	//cout<<program<<endl;

	int bytes=send(sockfd,program.c_str(),program.length(),0);
	if (bytes==program.length())
		return true;
	else return false;
}
bool  URDriver_program::send_reset_program(){
	//if (!ready_to_send_program) return false;
	string reset_program=
			"def resetProg():\n"//"popup(\"reset\")\n"
			"sleep(0.1)\n"
			"end\n";
	int bytes=send(sockfd,reset_program.c_str(),reset_program.length(),0);
	if (bytes==reset_program.length())
		return true;
	else return false;
	return true;
}
/*
 * Using this macro, only one component may live
 * in one library *and* you may *not* link this library
 * with another component library. Use
 * ORO_CREATE_COMPONENT_TYPE()
 * ORO_LIST_COMPONENT_TYPE(URDriver_program)
 * In case you want to link with another library that
 * already contains components.
 *
 * If you have put your component class
 * in a namespace, don't forget to add it here too:
 */
ORO_CREATE_COMPONENT(URDriver_program)
