#ifndef SRC_DESERIALIZE_STRUCTS_HPP_
#define SRC_DESERIALIZE_STRUCTS_HPP_
#pragma pack(1)
struct data_package_descriptor{
	int packageSize;
	unsigned char packageType;
} ;
#pragma pack(0)
//!This struct follows the indication of sheet 6 of the specfication given by UR.
/**For more info, look to
	 * [Client_Interface.xlsx](../Client_Interface.xlsx), page 6
	 *
	 */
#pragma pack(1)
struct data_robot_mode_v31{ //packageType = ROBOT_MODE_DATA = 0
	uint64_t timestamp;
	bool isRobotConnected;
	bool isRealRobotEnabled;
	bool isPowerOnRobot;
	bool isEmergencyStopped;
	bool isProtectiveStopped;
	bool isProgramRunning;
	bool isProgramPaused;
	unsigned char robotMode;
	unsigned char controlMode;
	double targetSpeedFraction;
	double speedScaling;
	double targetSpeedFractionLimit;  // ali
	unsigned char reserved;       // ali
};
#pragma pack(0)
#pragma pack(1)
struct data_joint_single{ //packageType = ROBOT_MODE_DATA = 0
	double q_actual;
	double q_target;
	double qd_actual;
	float I_actual;
	float V_actual;
	float T_motor;
	float T_micro;
	uint8_t jointMode; //ali
};
#pragma pack(0)
#pragma pack(1)
struct data_joints{ //packageType = ROBOT_MODE_DATA = 0
	data_joint_single joint[6];
};
#pragma pack(0)
#endif
