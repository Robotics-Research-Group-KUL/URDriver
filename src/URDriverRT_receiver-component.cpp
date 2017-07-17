#include "URDriverRT_receiver-component.hpp"
#include <rtt/Component.hpp>
#include <iostream>
using namespace RTT;
URDriverRT_receiver::URDriverRT_receiver(std::string const& name) : TaskContext(name,PreOperational)
, prop_address("192.168.1.102")
, port_number(30003)
, version_interface("3.0-3.1")
, v6(6,0.0)
{
	addProperty("port_number",port_number);
    addProperty("robot_address",prop_address);
	addProperty("version_interface",version_interface);
	addPort("q_actual_outport",q_actual_outport);
	addPort("qd_actual_outport",qd_actual_outport);
	addPort("time_outport",time_outport);
	addPort("period_outport",period_outport);

	q_actual_outport.setDataSample(v6);


	act = new RTT::extras::FileDescriptorActivity(os::HighestPriority);
	this->setActivity(act);
	act = dynamic_cast<RTT::extras::FileDescriptorActivity*>(this->getActivity());

}

bool URDriverRT_receiver::configureHook(){
	if (version_interface== "3.0-3.1")
		data_pointer=RTdata::Ptr(new RTdataV31());
	else if (version_interface== "Pre-3.0")
			data_pointer=RTdata::Ptr(new RTdataV18());
	else{

		Logger::In in(this->getName());
		log(Error)<<this->getName()<<":version_interface given is "<<version_interface
				<<" /n/t current accepted values are:"
				  "/n/t \"3.0-3.1\""
				  "/n/t \"Pre-3.0\""
				<< endlog();
		return false;
	}


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

bool URDriverRT_receiver::startHook(){


	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		Logger::In in(this->getName());
		log(Error)<<this->getName()<<": Connection failed!"<< endlog();
		return false;
	}
	log(Info)<<this->getName()<<": Connection OK!"<< endlog();
	//activity set watch
	act->watch(sockfd);
	act->setTimeout(2000);
	m_time_begin = os::TimeService::Instance()->getTicks();
	return true;
}

void URDriverRT_receiver::updateHook()
{
	m_time_passed = os::TimeService::Instance()->secondsSince(m_time_begin);
	m_time_begin = os::TimeService::Instance()->getTicks();
	period_outport.write(m_time_passed);
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


			int bytes_read= data_pointer->readRTData(sockfd);
			//todo control on numer of reads
			double d;

			if (bytes_read==0) return;
			int ok=data_pointer->getQ_actual(v6);
			if (ok==1) q_actual_outport.write(v6);

			ok=data_pointer->getQdot_actual(v6);
			if (ok==1) qd_actual_outport.write(v6);

			ok=data_pointer->getTime(d);
			if (ok==1) time_outport.write(d);

#ifndef NDEBUG
			cout<<"bites read:\t" << bytes_read <<endl;
			cout<<"time:\t"<<d<<endl;
#endif



		}

	}
}

void URDriverRT_receiver::stopHook() {
 act->clearAllWatches();
 close(sockfd);
}

void URDriverRT_receiver::cleanupHook() {

}

/*
 * Using this macro, only one component may live
 * in one library *and* you may *not* link this library
 * with another component library. Use
 * ORO_CREATE_COMPONENT_TYPE()
 * ORO_LIST_COMPONENT_TYPE(URDriverRT_receiver)
 * In case you want to link with another library that
 * already contains components.
 *
 * If you have put your component class
 * in a namespace, don't forget to add it here too:
 */
ORO_CREATE_COMPONENT(URDriverRT_receiver)
