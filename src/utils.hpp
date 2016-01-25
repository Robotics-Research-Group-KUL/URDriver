/*
 * utils.hpp
 *
 *  Created on: Dec 9, 2015
 *      Author: apertuscus
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>

#ifndef _UTILS_HPP_
#define _UTILS_HPP_
using namespace std;

inline void htond (double &x)
{
	int *Double_Overlay;
	int Holding_Buffer;
	Double_Overlay = (int *) &x;
	Holding_Buffer = Double_Overlay [0];
	Double_Overlay [0] = htonl (Double_Overlay [1]);
	Double_Overlay [1] = htonl (Holding_Buffer);
}
inline void ntohd (double &x)
{
	int *Double_Overlay;
	int Holding_Buffer;
	Double_Overlay = (int *) &x;
	Holding_Buffer = Double_Overlay [0];
	Double_Overlay [0] = ntohl (Double_Overlay [1]);
	Double_Overlay [1] = ntohl (Holding_Buffer);
}

//! Swap in place a data type, for converting btw little and big endian
   /*!
     \param d data to be swapped
     \return number of bytes of the data swapped, return 0 in case of error
   */
template <class T> int swap (T& d)
{
	int n=sizeof(d);
	T a;
	unsigned char *dst = (unsigned char *)&a;
	unsigned char *src = (unsigned char *)&d;
	switch (n) {
	case 1:
		break;
	case 4:
		dst[0] = src[3];
		dst[1] = src[2];
		dst[2] = src[1];
		dst[3] = src[0];
		break;
	case 8:
		dst[0] = src[7];
		dst[1] = src[6];
		dst[2] = src[5];
		dst[3] = src[4];
		dst[4] = src[3];
		dst[5] = src[2];
		dst[6] = src[1];
		dst[7] = src[0];
		break;
	default:
		return 0;
	}
	d=a;
	return n;
};

inline bool copyvector( const double vin [],std::vector<double>&vout, int size)
{
	if (vout.size()!=size) return false;
	for (int i=0;i<size;i++)
		vout[i]=vin[i];
	return true;
}
#endif
