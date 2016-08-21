/*************************************************************************
	> File Name: client.cpp
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Sun 21 Aug 2016 06:36:17 PM CST
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

int proc_forward( int in, const char *ip, const char *port )
{
	int out = socket( AF_INET, SOCK_STREAM, 0 );
	assert( out != -1 );
	
	sockaddr_in raddr;
	memset( &raddr, 0, sizeof( raddr ) );
	raddr.sin_family = AF_INET;
	raddr.sin_addr.s_addr = inet_addr( ip );
	assert( 0 == connect( out, (sockaddr*)&raddr, sizeof( raddr ) ) );


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
	if( argc != 4 )
	{
		cout << "Usage: ./client port ip peer_port" << endl;
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
				return proc_forward( in, argv[2], argv[3] );
			}
		}
	}

	return 0;
}
