/*************************************************************************
	> File Name: transfer.cpp
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Sun 21 Aug 2016 06:00:32 PM CST
 ************************************************************************/

#include "transfer.h"

void *transfer( void *p )
{
	TParam *param = (TParam*)p;
	int in = param->in;
	int out = param->out;
	pthread_t bro = param->bro;
	char buf[BUF_LEN];
	memset( buf, '\0', BUF_LEN );

	while(1)
	{
		int ret = recv_n( in, buf, BUF_LEN );
		if( ret <= 0 )
		{
			break;
		}
		ret = send_n( out, buf, BUF_LEN );
		if( ret < 0 )
		{
			break;
		}
	}
}
