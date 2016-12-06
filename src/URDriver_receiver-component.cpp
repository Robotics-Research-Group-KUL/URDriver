#include "URDriver_receiver-component.hpp"
#include <rtt/Component.hpp>
#include <iostream>
using namespace RTT;
URDriver_receiver::URDriver_receiver(std::string const& name) : TaskContext(name,PreOperational)
, v6(6,0.0)
, prop_address("192.168.1.102")
, port_number(30002)
{
	addProperty("port_number",port_number);
    addProperty("robot_address",prop_address);

	/// robot_mode_value
	this->provides("robot_mode_value")->doc("Ports giving access to the state of the robot.");
	this->provides("robot_mode_value")->addPort("isProgramRunning",isProgramRunning);
	this->provides("robot_mode_value")->addPort("isProgramPaused",isProgramPaused);
	this->provides("robot_mode_value")->addPort("IsEmergencyStopped",IsEmergencyStopped);

	addPort("bytes_outport",bytes_outport);

	data_pointer=URdata::Ptr(new URdataV31());
	//q_qctual_outport.setDataSample(v6);


	act = new RTT::extras::FileDescriptorActivity(os::HighestPriority);
	this->setActivity(act);
	act = dynamic_cast<RTT::extras::FileDescriptorActivity*>(this->getActivity());
}

bool URDriver_receiver::configureHook(){


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
    if(inet_pton(AF_INET, prop_address.c_str(), &serv_addr.sin_addr)<=0)
	{
		Logger::In in(this->getName());
        log(Error)<<this->getName()<<":the string "<<prop_address
				<<" is not a good formatted string for address ( like 127.0.0.1)"
				<< endlog();
		///add log from other file
		return false;
	}
	return true;
}

bool URDriver_receiver::startHook(){

	Logger::In in(this->getName());
	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		log(Error)<<this->getName()<<": Connection failed!"<< endlog();
		return false;
	}
	log(Info)<<this->getName()<<": Connection OK!"<< endlog();
	act->watch(sockfd);
	act->setTimeout(2000);

	return true;
}

void URDriver_receiver::updateHook(){
	if(act->hasError()){
		Logger::In in(this->getName());
		log(Error)  <<this->getName()<<" socket error - unwatching all sockets. restart the component" << endlog();
		act->clearAllWatches();
		close(sockfd);
		this->stop();
		this->cleanup();
	}
	else if (act->hasTimeout()){
		Logger::In in(this->getName());
		log(Error)  <<this->getName()<<" socket timeout" << endlog();

	}
	else{
		if(act->isUpdated(sockfd)){

			int bytes_read= data_pointer->readURdata(sockfd);

			bytes_outport.write(bytes_read);
			bool ret;
			if (data_pointer->getIsProgramRunning(ret))
				isProgramRunning.write(ret);
			if (data_pointer->getIsProgramPaused(ret))
				isProgramPaused.write(ret);
			if (data_pointer->getIsEmergencyStopped(ret))
				IsEmergencyStopped.write(ret);


		}
	}
}
void URDriver_receiver::stopHook() {
	act->clearAllWatches();
	close(sockfd);
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
