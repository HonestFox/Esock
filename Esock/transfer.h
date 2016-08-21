/*************************************************************************
	> File Name: transfer.h
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Sun 21 Aug 2016 06:00:28 PM CST
 ************************************************************************/
#pragma once
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

const int BUF_LEN = 1048576;

void* transfer( void *p );

struct TParam
{
	int in;
	int out;
	pthread_t bro;
};
