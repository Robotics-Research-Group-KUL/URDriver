#include "URDriver_program-component.hpp"
#include <rtt/Component.hpp>
#include <iostream>
#include <fstream>      // std::ifstream
using namespace RTT;
URDriver_program::URDriver_program(std::string const& name) : TaskContext(name,PreOperational)
, prop_adress("192.168.1.102")
, port_number(30002)
, ready_to_send_program(false)
, reverse_port_number(50001)
{
	addProperty("port_number",port_number);
	addProperty("reverse_port_number",reverse_port_number);
	addProperty("prop_adress",prop_adress).doc("ip address robot");
    addOperation("send_reset_program", &URDriver_program::send_reset_program, this, RTT::OwnThread);
    addOperation("send_program", &URDriver_program::send_program, this, RTT::OwnThread);
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
	    memset(&program_server_addr, '0', sizeof(program_server_addr));

	    program_server_addr.sin_family = AF_INET;
	    program_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	    program_server_addr.sin_port = htons(reverse_port_number);

	    bind(listenfd, (struct sockaddr*)&program_server_addr, sizeof(program_server_addr));

	    listen(listenfd, 10);



	ready_to_send_program=true;
	return true;


}
bool  URDriver_program::send_program(){
	if (!ready_to_send_program) return false;
	std::ifstream t("prog");
std::stringstream buffer;
buffer << t.rdbuf();
string program=buffer.str();
	cout<<program<<endl;
	int bytes=send(sockfd,program.c_str(),program.length(),0);
	cout<<"bytes: "<<bytes<<"  expected: "<<program.length()<<endl;
	return true;
}
bool  URDriver_program::send_reset_program(){
	if (!ready_to_send_program) return false;
	string reset_program=
			"def resetProg():\n"
			"\tpopup(\"RESET\")\n"
			"end\n";
	/*		string reset_program=
					"def resetProg():\n"
					"\tmovej([1.5,-0.4,-1.57,0,0,0], 3, 0.75, 1.0)\n"
					"\tpopup(\"CIAO\")\n"
					"end\n";*/

	int bytes=send(sockfd,reset_program.c_str(),reset_program.length(),0);
	cout<<"bytes: "<<bytes<<"  expected: "<<reset_program.length()<<endl;
	std::cout << reset_program << std::endl;
	return true;
}
bool URDriver_program::startHook(){

	return true;
}

void URDriver_program::updateHook(){


}

void URDriver_program::stopHook() {

}

void URDriver_program::cleanupHook() {

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
