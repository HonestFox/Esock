/*************************************************************************
	> File Name: server.cpp
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Sun 21 Aug 2016 04:26:03 PM CST
 ************************************************************************/

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "protocol.h"
#include "transfer.h"
#include "common.h"


using namespace std;

static int startup( int port, char *ip = NULL, bool IsServer = true)
{
	//Create
	int sock = socket( AF_INET, SOCK_STREAM, 0 );
	if( sock < 0 )
	{
		cout << "Create socket error !" << endl;
		exit(2);
	}
	int opt = 1;
	setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) );


	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons( port );
	if( IsServer )
	{
		local.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		local.sin_addr.s_addr = inet_addr(ip);	
	}

	//Bind
	if( bind( sock, ( sockaddr* )&local, sizeof(local) ) < 0 )
	{
		cout << "Bind socket error !" << endl;
		exit(3);
	}

	//Listen
	if( listen( sock, 5 ) )
	{
		cout << "Listen error !" << endl;
		exit(4);
	}
	return sock;

}

int proc_socks( int in )
{
	SKReq sreq;
	assert( 0 == recv_n( in, (char*)&sreq, sizeof(sreq) ) );
	assert( sreq.ver == 5 && sreq.n == 1 );
	char methods[8];
	assert( 0 == recv_n( in, methods, sreq.n ) );
	assert( 0 == methods[0] );
	SKRep rep;
	rep.ver = 5;
	rep.m = 0;
	assert( 0 == send_n( in, (char*)&rep, sizeof(rep) ) );

	sockaddr_in out_addr;
	memset( &out_addr, 0, sizeof( out_addr ) );
	out_addr.sin_family = AF_INET;

	AddrReq areq;
	assert( 0 == recv_n( in, (char*)&areq, sizeof( areq ) ) );
	assert( areq.ver == 5 && areq.cmd == 1 && areq.rsv == 0 );

	if( areq.atype == 1 )
	{
		//ipv4
		assert( 0 == recv_n( in, (char*)&out_addr.sin_addr.s_addr, sizeof( out_addr.sin_addr.s_addr ) ) );
	}
	else if( areq.atype == 3 )
	{
		//domain
		char dlen;
		assert( 0 == recv_n( in, (char*)&dlen, 1 ) );
		char domain[256];
		assert( 0 == recv_n( in, domain, dlen ) );
		domain[dlen] = 0;
		hostent *host = gethostbyname( domain );
		assert( host && host->h_addrtype == AF_INET && host->h_length > 0 );
		memcpy( &out_addr.sin_addr.s_addr, host->h_addr_list[0], sizeof( out_addr.sin_addr.s_addr ) );
	}
	else
	{
		assert(0);
	}
	
	assert( 0 == recv_n( in, (char*)&out_addr.sin_port, sizeof( out_addr.sin_port ) ) );

	//Begin Connect
	int out = socket( AF_INET, SOCK_STREAM, 0 );
	assert( out != -1 );
	assert( 0 == connect( out, (sockaddr*)&out_addr, sizeof( out_addr ) ) );

	//Come Back Message
	AddrRep arep;
	arep.ver = 5;
	arep.cmd = 0;
	arep.rsv = 0;
	arep.atype = 1;
	sockaddr_in local_addr;
	socklen_t slen = sizeof( local_addr );
	assert( 0 == getsockname( out, (sockaddr*)&local_addr, &slen ) );
	memcpy( &arep.addr, &local_addr.sin_addr, sizeof( local_addr.sin_addr ) );
	memcpy( &arep.port, &local_addr.sin_port, sizeof( local_addr.sin_port ) );
	assert( 0 == send_n( in, (char*)&arep, sizeof( arep ) ) );
	
	//Begin Transport
	TParam up = { in, out , 0 };
	TParam down = { out, in, 0 };
	assert( pthread_create( &down.bro, NULL, transfer, &up ) );
	assert( pthread_create( &up.bro, NULL, transfer, &down ) );
	pthread_join( up.bro, NULL );
	pthread_join( down.bro, NULL );
	return 0;
}

int main( int argc, char *argv[] )
{
	if( argc != 2 )
	{
		cout << "Usage: ./server port" << endl;
		return 1;
	}

	int lsnr = startup( atoi( argv[1] ) );

	while( 1 )
	{
		sockaddr caddr;
		socklen_t slen = sizeof( caddr );
		int in = accept( lsnr, &caddr, &slen );
		assert( in != -1 );

		pid_t pid = fork();
		assert( pid >= 0 );
		if( pid > 0 )  //grand father
		{
			waitpid( pid, NULL, 0 );
			close( in );
		}
		else
		{
			pid_t pid2 = fork();
			assert( pid2 >= 0 );
			if( pid2 > 0 )	//father
			{
				exit(0);	
			}
			else	//son
			{
				close( lsnr );
				return proc_socks( in );
			}
		}
	}

	return 0;
}
