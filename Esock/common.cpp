/*************************************************************************
	> File Name: common.cpp
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Sun 21 Aug 2016 05:18:39 PM CST
 ************************************************************************/
#include "common.h"

int recv_n( int sock, char *buf, int len )
{
	char *cur = buf;
	int left = len;
	while( left > 0 )
	{
		int ret = recv( sock, cur, left, 0 );
		if( ret <= 0 )
		{
			return -1;
		}
		cur += ret;
		left -= ret;
	}
	enx( buf, len );
	return 0;
}

int send_n( int sock, char *buf, int len )
{
	enx( buf, len );
	const char *cur = buf;
	int left = len;
	while( left > 0 )
	{
		int ret = send( sock, cur, left, 0 );
		if( ret < 0 )
		{
			return -1;
		}
		cur += ret;
		left -= ret;
	}
	return 0;
}

void enx( char *data, int len )
{
	for(int i = 0; i < len; ++i )
	{
		data[i] ^= 7;
	}
}
