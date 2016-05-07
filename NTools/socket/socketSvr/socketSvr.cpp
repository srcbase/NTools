// socketSvr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <socket/sockme.h>

int _tmain(int argc, _TCHAR* argv[])
{
	if(!acg::socket::init(2,2))
	{
		std::cout << "start up socket faile" << std::endl;
		acg::socket::clean();
	}
	acg::socket svr;
	//svr.serviceDaytimeUDP();
	//svr.serviceECHOTCP();
	svr.serviceHeartBeat(6789);

	std::cout << "clean up socket" << std::endl;

	acg::socket::clean();
	int over;
	std::cin >> over;
	return 0;
}

