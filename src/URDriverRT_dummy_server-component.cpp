#include "URDriverRT_dummy_server-component.hpp"
#include <rtt/Component.hpp>
#include <iostream>
#include <fstream>      // std::ifstream
#include "utils.hpp"      // std::ifstream
#include <libexplain/bind.h>
#include <signal.h>
#include "Deserialize.hpp"
using namespace RTT;
URDriverRT_dummy_server::URDriverRT_dummy_server(std::string const& name) : TaskContext(name,PreOperational)
      , reverse_port_number(30003)
{

	addProperty("reverse_port_number",reverse_port_number);
	addOperation("open_server",
		     &URDriverRT_dummy_server::open_server, this, RTT::OwnThread);

	buffer.reserve(1024);


}



void signal_callback_handler(int signum){

	printf("Caught signal SIGPIPE %d\n",signum);
}
bool URDriverRT_dummy_server::configureHook(){




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
	program_server_addr.sin_port = htons(reverse_port_number);




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

	open_server();

	return true;
}
bool URDriverRT_dummy_server::open_server()
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

	return true;
}

bool URDriverRT_dummy_server::startHook(){

	time_now=0;
	return true;
}




void URDriverRT_dummy_server::updateHook(){

	signal(SIGPIPE, signal_callback_handler);
	data_frame.Message_Size=ntohl(sizeof(data_frame));
	time_now+=getPeriod();
	data_frame.Time=time_now;
	ntohd(data_frame.Time);

	int n = write(newsockfd,&data_frame,sizeof(data_frame));
	if (n < 0)
	{
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<": error writing socket."<< endlog();

	}
	else
	{
		log(Info)<<this->getName()<<": sent data: bytes "<<n<< endlog();
		log(Info)<<this->getName()<<": sizeof(data_frame) bytes "<<sizeof(data_frame)<< endlog();

	}



}


/*
 * Using this macro, only one component may live
 * in one library *and* you may *not* link this library
 * with another component library. Use
 * ORO_CREATE_COMPONENT_TYPE()
 * ORO_LIST_COMPONENT_TYPE(URDriverRT_dummy_server)
 * In case you want to link with another library that
 * already contains components.
 *
 * If you have put your component class
 * in a namespace, don't forget to add it here too:
 */
ORO_CREATE_COMPONENT(URDriverRT_dummy_server)
