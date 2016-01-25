#include "URDriver_receiver-component.hpp"
#include <rtt/Component.hpp>
#include <iostream>
using namespace RTT;
URDriver_receiver::URDriver_receiver(std::string const& name) : TaskContext(name)
, v6(6,0.0)
, prop_adress("192.168.1.102")
, port_number(30002)
{
	addProperty("port_number",port_number);
	addProperty("prop_adress",prop_adress);
	//addPort("q_qctual_outport",q_qctual_outport);
	//addPort("time_outport",time_outport);
	data_pointer=URdata::Ptr(new URdataV31());
	//q_qctual_outport.setDataSample(v6);
}

bool URDriver_receiver::configureHook(){

	cout<<"configure"<<endl;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<": not able to open the socket..." << endlog();
		return false;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port=htons(port_number);

	//Convert from presentation format to an Internet number
	if(inet_pton(AF_INET, prop_adress.c_str(), &serv_addr.sin_addr)<=0)
	{
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<":the string "<<prop_adress
				<<" is not a good formatted string for address ( like 127.0.0.1)"
				<< endlog();
		///add log from other file
		return false;
	}
	return true;
}

bool URDriver_receiver::startHook(){

	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		log(Error)<<this->getName()<<": Connection failed!"<< endlog();
		return false;
	}
	log(Info)<<this->getName()<<": Connection OK!"<< endlog();
	return true;
}

void URDriver_receiver::updateHook(){
	int bytes_read= data_pointer->readURdata(sockfd);
	double d;
	/*bool ok=data_pointer->getQ_actual(v6);

	if (ok)
		q_qctual_outport.write(v6);
	ok=data_pointer->getTime(d);

	if (ok)
		time_outport.write(d);*/
}

void URDriver_receiver::stopHook() {

}

void URDriver_receiver::cleanupHook() {

}

/*
 * Using this macro, only one component may live
 * in one library *and* you may *not* link this library
 * with another component library. Use
 * ORO_CREATE_COMPONENT_TYPE()
 * ORO_LIST_COMPONENT_TYPE(URDriver_receiver)
 * In case you want to link with another library that
 * already contains components.
 *
 * If you have put your component class
 * in a namespace, don't forget to add it here too:
 */
ORO_CREATE_COMPONENT(URDriver_receiver)
