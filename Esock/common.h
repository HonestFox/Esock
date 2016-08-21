/*************************************************************************
	> File Name: common.h
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Sun 21 Aug 2016 05:18:00 PM CST
 ************************************************************************/
#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

int recv_n( int sock, char *buf, int len );
int send_n( int sock, char *buf, int len );
void enx( char *data, int len );
