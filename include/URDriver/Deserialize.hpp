/*
 * Deserialize.hpp
 *
 *  Created on: Dec 9, 2015
 *      Author: apertuscus
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include "utils.hpp"
#include "Deserialize_structs.hpp"
#ifndef SRC_DESERIALIZE_HPP_
#define SRC_DESERIALIZE_HPP_
using namespace std;


class URdata
{
protected:
	std::string type;

	bool configured;
public:
	URdata(): type("UNSET"),
		configured(false){}


	typedef boost::shared_ptr<URdata> Ptr;
	virtual ~URdata(){}
	/**
	 * @brief read the data structure from the data
	 * @param sockfd socket descriptor
	 * @return  negative with an error (to be doumented in specific implementations)
	 *		otherwise the bytes read
	 */
	virtual int readURdata(const int sockfd)=0;

	/** @name Getter functions
	 *  These functions all returns -1 if they are not
	 *  implemented in the derived class,
	 * Other errors (negative values) can depend on specific implementation
	 */
	///@{

	virtual int getType(string  &t)const{t=type; return -1;}


	virtual int getIsRobotConnected  ( bool&t)const {return -1;}
	virtual int getIsEmergencyStopped  ( bool&t)const {return -1;}
	virtual int getIsProgramRunning  ( bool&t)const {return -1;}
	virtual int getIsProgramPaused  ( bool&t)const {return -1;}


	///@}

};




/** @name Class that are Ok for most of the versions
 */
///@{
class joints{
	static const int id=0;
public:
	data_joints data;
	int read_data(const int sockfd)
	{
		int n = read(sockfd,&data,sizeof(data));
		for (int i=0;i<6;i++)
		{
			swap(data.joint[i].q_actual);
			swap(data.joint[i].q_target);
			swap(data.joint[i].qd_actual);
			swap(data.joint[i].I_actual);
			swap(data.joint[i].V_actual);
			swap(data.joint[i].T_motor);
			swap(data.joint[i].T_micro);
			swap(data.joint[i].jointMode);
		}
		return n;
	}
} ;

class pkg_descriptor{
public:
	data_package_descriptor data;
	int read_data(const int sockfd)
	{
		int n = read(sockfd,&data,sizeof(data));
		data.packageSize=ntohl(data.packageSize);
		return n;
	}
};
///@}


class robot_mode_v31{
	static const int id=0;
public:
	data_robot_mode_v31 data;
	int read_data(const int sockfd)
	{
		int n = read(sockfd,&data,sizeof(data));
		swap(data.timestamp);
		ntohd(data.targetSpeedFraction);
		ntohd(data.speedScaling);
		return n;
	}
} ;






class URdataV31:public URdata
{
public:
	URdataV31(){type="3.1";}
	robot_mode_v31 robot_mode_value;
	joints joints_value;
	char buffer[4096];
	int readURdata(const int sockfd)
	{
		//first read an integer to get the size of the package.
		pkg_descriptor pkg_descr;


		int n = pkg_descr.read_data(sockfd);
		int byte_total=pkg_descr.data.packageSize;
		int bytes_left =byte_total;
		bytes_left-=n;
#ifndef NDEBUG
		cout<<"read 1: bites read: "<<n
		   <<"\n\tpkg_descr.packageSize (byte total):"<<pkg_descr.data.packageSize
		  <<"\n\tpkg_descr.packageType: "<< (int)pkg_descr.data.packageType
		 <<"\n\tbytes_left= "<<bytes_left<<endl;
#endif
		if ((int)pkg_descr.data.packageType!=16)
		{//error
			n = read(sockfd,&buffer,sizeof(buffer));
			return -1;
		}

		int last_read=-1;
		bool ok=true;
		while(bytes_left>0)
		{
			n = pkg_descr.read_data(sockfd);
			bytes_left-=n;
#ifndef NDEBUG
			cout<<"\n\tpkg_descr.packageSize (byte total):"<<pkg_descr.data.packageSize
			   <<"\n\tpkg_descr.packageType: "<< (int)pkg_descr.data.packageType<<bytes_left<<endl;
#endif
			if(last_read<=(int)pkg_descr.data.packageType)
			{
				cout<<"last_read:\t"<<last_read<<"\tread: "<<(int)pkg_descr.data.packageType<<endl;
			}
			else last_read=(int)pkg_descr.data.packageType;

			switch((int)pkg_descr.data.packageType) {
			case 0:
				n=  robot_mode_value.read_data(sockfd);
				bytes_left-=n;
#ifndef NDEBUG
				cout<<"read package: bites read: "<<n
				   <<"\n\trobot_mode_value :"<<robot_mode_value.data.timestamp
				  <<endl;
#endif
				break;
			case 1:
				n=  joints_value.read_data(sockfd);
				bytes_left-=n;
#ifndef NDEBUG
				cout<<"read robot_mode_value: bites read: "<<n
				   <<"\n\t j1:"<<joints_value.data.joint[0].q_actual/3.14*180
				  <<"\n\t j2:"<<joints_value.data.joint[1].q_actual/3.14*180
				 <<"\n\t j3:"<<joints_value.data.joint[2].q_actual/3.14*180
				<<"\n\t j4:"<<joints_value.data.joint[3].q_actual/3.14*180
				<<"\n\t j5:"<<joints_value.data.joint[4].q_actual/3.14*180
				<<"\n\t j6:"<<joints_value.data.joint[5].q_actual/3.14*180
				<<endl;
#endif
			default:
				ok=false;
				break;
			}
			if (!ok) break;



		}
		// read the rest
		n = read(sockfd,&buffer,bytes_left);
#ifndef NDEBUG
		cout<<"END: bites read to flush: "<<n
		   <<"\n\t expected:"<<bytes_left<<endl;
#endif

		return byte_total;
	}

	 int getType(string  &t)const{t=type; return -1;}


	int getIsRobotConnected  ( bool&t)const
	{
		t=robot_mode_value.data.isRobotConnected;
		return 1;
	}

	int getIsEmergencyStopped  ( bool&t)const
	{
		t=robot_mode_value.data.isEmergencyStopped;
		return 1;
	}
	int getIsProgramRunning  ( bool&t)const
	{
		t=robot_mode_value.data.isProgramRunning;
		return 1;
	}
	int getIsProgramPaused  ( bool&t)const
	{
		t=robot_mode_value.data.isProgramPaused;
		return 1;
	}
private:



};




#endif /* SRC_RTDESERIALIZE_HPP_ */
