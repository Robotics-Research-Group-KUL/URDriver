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
	///Getter function for TCP force value
	virtual int getTCP_Force  ( vector<double>&t)const {return -1;}
	///@}

};

//!This class implements the protocol for controller form v5.10 and v5.11.
/**
 * details here
 */
class RTdataV511:public RTdata
{
public:
	RTdataV511(){size=1220;type="5.10 to 5.11";}
	int getNominalSize  ( int&t)const {t=sizeof(data);return 1;}
	int getTime  ( double&t)const {t=data.Time;return 1;}
	int getQ_actual  ( vector<double>&q)const
	{if (copyvector(data.q_actual,q,6)) return 1; return 0;}
	int getQdot_actual  ( vector<double>&qd)const
	{if (copyvector(data.qd_actual,qd,6)) return 1; return 0;}
	int getTCP_Force  ( vector<double>&fv)const
	{if (copyvector(data.TCP_force,fv,6)) return 1; return 0;}


	/**
	 * Reads the data from the socket and fill in the internal data structure
	 * @param sockfd socket identifier
	 * @return number of bytes read
	 */
	int readRTData(const int sockfd)
	{

		int n = read(sockfd,&data,sizeof(data));

		// std::cout << "sizeof(data):" <<std::endl;
		// std::cout << sizeof(data) <<std::endl;

		data.Message_Size=ntohl(data.Message_Size);
		double * pointer=&data.Time;

		for (int i=0;i<139;i++)
		{

			ntohd(pointer[i]);
		}

		return n;
	};
private:

	//!This struct follows the indication of the specfication given by UR.
	/**For more info, look to
	 * (../doc/ClientInterfaces_Realtime.pdf), page 9
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
		Test_value	, //Check
		Robot_Mode	;
		double  Joint_Modes[6];
		double Safety_Mode	;
		double UNUSED1[6]	;//6 Safety status
		double Tool_Accelerometer_values[3];//3
		double UNUSED2	[6];//6 Check
		double Speed_scaling,
		Linear_momentum_norm,
		UNUSED3,
		UNUSED4,
		V_main	,
		V_robot,
		I_robot;
		double V_actual[6],//6
		Digital_outputs,
		Program_state,
		Elbow_position[3],
		Elbow_velocity[3];
		double Safety_Status,
		UNUSED5,
		UNUSED6,
		UNUSED7,
		Payload_Mass;
		double Payload_CoG[3];
		double Payload_Inertia[6];
	} ;

#pragma pack(0)
	data_struct data;
};


//!This class implements the protocol for controller form v5.4 and v5.9.
/**
 * details here
 */
class RTdataV59:public RTdata
{
public:
	RTdataV59(){size=1116;type="5.4 to 5.9";}
	int getNominalSize  ( int&t)const {t=sizeof(data);return 1;}
	int getTime  ( double&t)const {t=data.Time;return 1;}
	int getQ_actual  ( vector<double>&q)const
	{if (copyvector(data.q_actual,q,6)) return 1; return 0;}
	int getQdot_actual  ( vector<double>&qd)const
	{if (copyvector(data.qd_actual,qd,6)) return 1; return 0;}
	int getTCP_Force  ( vector<double>&fv)const
	{if (copyvector(data.TCP_force,fv,6)) return 1; return 0;}


	/**
	 * Reads the data from the socket and fill in the internal data structure
	 * @param sockfd socket identifier
	 * @return number of bytes read
	 */
	int readRTData(const int sockfd)
	{

		int n = read(sockfd,&data,sizeof(data));

		// std::cout << "sizeof(data):" <<std::endl;
		// std::cout << sizeof(data) <<std::endl;

		data.Message_Size=ntohl(data.Message_Size);
		double * pointer=&data.Time;

		for (int i=0;i<139;i++)
		{

			ntohd(pointer[i]);
		}

		return n;
	};
private:

	//!This struct follows the indication of sheet 43 of the specfication given by UR.
	/**For more info, look to
	 * (../Client_InterfaceV3.14andV5.9.xlsx), page 43
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
		Test_value	, //Check
		Robot_Mode	;
		double  Joint_Modes[6];
		double Safety_Mode	;
		double UNUSED1[6]	;//6 Safety status
		double Tool_Accelerometer_values[3];//3
		double UNUSED2	[6];//6 Check
		double Speed_scaling,
		Linear_momentum_norm,
		UNUSED3,
		UNUSED4,
		V_main	,
		V_robot,
		I_robot;
		double V_actual[6],//6
		Digital_outputs,
		Program_state,
		Elbow_position[3],
		Elbow_velocity[3],
		Safety_Status;
	} ;

#pragma pack(0)
	data_struct data;
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
	int getTCP_Force  ( vector<double>&fv)const
	{if (copyvector(data.TCP_force,fv,6)) return 1; return 0;}


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




//!This class implements the protocol for controller Pre-3.0.
/**
 * details here
 */
class RTdataV18:public RTdata
{
public:
	RTdataV18(){size=764;type="Pre-3.0";}
	int getNominalSize  ( int&t)const {t=sizeof(data);return 1;}
	int getTime  ( double&t)const {t=data.Time;return 1;}
	int getQ_actual  ( vector<double>&q)const
	{if (copyvector(data.q_actual,q,6)) return 1; return 0;}
	int getQdot_actual  ( vector<double>&qd)const
	{if (copyvector(data.qd_actual,qd,6)) return 1; return 0;}
	int getTCP_Force  ( vector<double>&fv)const
	{if (copyvector(data.TCP_force,fv,6)) return 1; return 0;}


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

		for (int i=0;i<95;i++)
			ntohd(pointer[i]);

		return n;
	};
private:

	//!This struct follows the indication of sheet 16 of the specfication given by UR.
	/**For more info, look to
	 * [Client_Interface.xlsx](../Client_Interface.xlsx), page 15
	 *
	 * note from the documentation:
	 * `` If it is experienced that less than 756 bytes
	 * are received, the protocol for the actual received
	 * bytes also follows the structure listed above, only
	 * not containing the entries at leading up the 756th byte. ''
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
		Tool_Accelerometer_values	[3],
		UNUSED[15],
		TCP_force	[6],
		Tool_vector	[6],
		TCP_speed   [6]	;
		double Digital_input_bits;
		double Motor_temperatures[6];
		double Controller_Timer		,
		Test_value	,
		Robot_Mode	;
		double  Joint_Modes[6];
	} ;


#pragma pack(0)
	data_struct data;
};



#endif /* SRC_RTDESERIALIZE_HPP_ */