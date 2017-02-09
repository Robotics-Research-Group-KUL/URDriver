/*
 * RTDeserialize.hpp
 *
 *  Created on: Dec 9, 2015
 *      Author: apertuscus
 */

#include <sys/socket.h>
#include <arpa/inet.h>

#ifndef SRC_RTDESERIALIZE_HPP_
#define SRC_RTDESERIALIZE_HPP_
using namespace std;

#include "utils.hpp"

class RTdata
{
protected:
	std::string type;
	int size;

	bool configured;
public:
	RTdata(): type("UNSET"),
	size(-1),configured(false){}

	typedef boost::shared_ptr<RTdata> Ptr;
	virtual ~RTdata(){}

	/**
	 * @brief read the data structure from the data
	 * @param sockfd socket descriptor
	 * @return  negative with an error (to be doumented in specific implementations)
	 *		otherwise the bytes read
	 */
	virtual int readRTData(int)=0;
	/** @name Getter functions
	 *  These functions all returns false if they are not
	 *  implemented in the derived class, or the size of vectors
	 *  is wrong.
	 */
	///@{
	///Getter function for time
	virtual int getType(string  &t)const{t=type; return -1;}
	//! Getter function for expected size in byte of the struct given by the robot.
	/**
	 * Useful for checking after calling   RTdata#readRTData
	 */
	virtual int getNominalSize(int  &t)const{t=size; return -1;}
	///Getter function for time
	virtual int getTime  ( double&t)const {return -1;}
	///Getter function for joint value
	virtual int getQ_actual  ( vector<double>&t)const {return -1;}
	///Getter function for joint velocity value
	virtual int getQdot_actual  ( vector<double>&t)const {return -1;}
	///@}

};


//!This class implements the protocol for controller v3.0 and v3.1.
/**
 * details here
 */
class RTdataV31:public RTdata
{
public:
	RTdataV31(){size=1044;type="3.0 and 3.1";}
	int getNominalSize  ( int&t)const {t=sizeof(data);return 1;}
	int getTime  ( double&t)const {t=data.Time;return 1;}
	int getQ_actual  ( vector<double>&q)const
	{if (copyvector(data.q_actual,q,6)) return 1; return 0;}
	int getQdot_actual  ( vector<double>&qd)const
	{if (copyvector(data.qd_actual,qd,6)) return 1; return 0;}


	/**
	 * Reads the data from the socket and fill in the internal data structure
	 * @param sockfd socket identifier
	 * @return number of bytes read
	 */
	int readRTData(const int sockfd)
	{


		int n = read(sockfd,&data,sizeof(data));



		data.Message_Size=ntohl(data.Message_Size);
		double * pointer=&data.Time;

		for (int i=0;i<130;i++)
		{

			ntohd(pointer[i]);
		}

		return n;
	};
private:

	//!This struct follows the indication of sheet 16 of the specfication given by UR.
	/**For more info, look to
	 * [Client_Interface.xlsx](../Client_Interface.xlsx), page 16
	 *
	 */
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

#pragma pack(0)
	data_struct data;
};
/*
class RTDeserialize
{
private:
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;

public:
	//for now, i will avoid the test to choose between the several models.
	typedef boost::shared_ptr<RTDeserialize> Ptr;
	RTDeserialize(){
		data_pointer=RTdata::Ptr(new RTdataV31());
	}

	RTdata::Ptr data_pointer;
	bool setConnection(int port_number,string prop_address)
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			return false;
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port=htons(port_number);

		//Convert from presentation format of an Internet number
		if(inet_pton(AF_INET, prop_address.c_str(), &serv_addr.sin_addr)<=0)
		{

			return false;
		}
		return true;
	};
	bool startConnection(int port_number,string prop_address){
		if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			return false;
		return true;
	}
	int UpdateValues(){
		return data_pointer->readRTData(sockfd);
	}
};
 */



#endif /* SRC_RTDESERIALIZE_HPP_ */
