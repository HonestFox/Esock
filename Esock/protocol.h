/*************************************************************************
	> File Name: protocol.h
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Sun 21 Aug 2016 05:14:18 PM CST
 ************************************************************************/
#pragma once
#include<iostream>

#pragma pack(push)
#pragma pack(1)

struct SKReq
{
	char ver;
	char n;
};

struct SKRep
{
	char ver;
	char m;
};

struct AddrReq
{
	char ver;
	char cmd;
	char rsv;
	char atype;
};

struct AddrRep
{
	char ver;
	char cmd;
	char rsv;
	char atype;
	int addr;
	short port;
};
#pragma pack(pop)
