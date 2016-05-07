// testSocket.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <socket/sockme.h>
#include <ac/normal.h>
int _tmain(int argc, _TCHAR* argv[])
{
	in_addr addr = {0};
	if(!acg::socket::init(2,2))
	{
		std::cout << "start up socket faile" << std::endl;
		acg::socket::clean();
	}
	acg::socket client;
	//std::cout << "---- 启动循环无连接的daytime测试 ----------------------------------" << std::endl;
	//if(client.connect("192.168.1.51", "daytime", "udp"))
	//{
	//	std::time_t now = {0};
	//	if(client.daytime(now))
	//	{
	//		std::tm *plocalTM = std::localtime(&now);
	//		char str[40] = {0};
	//		std::sprintf(str, "%d-%02d-%02d %02d:%02d:%02d", plocalTM->tm_year+1900, plocalTM->tm_mon+1, plocalTM->tm_mday, plocalTM->tm_hour, plocalTM->tm_min, plocalTM->tm_sec);
	//		std::cout << "服务器时间为：" << str << std::endl;
	//	}
	//	else
	//		std::cout << "失败！" << std::endl;
	//}

	//std::cout << "---- 启动并发面向连接的echo测试 ----------------------------------" << std::endl;
	//char buf[1024] = {0};
	//acg::socket::ArrayType rcdata;
	//if(client.connect("192.168.1.51", "echo", "tcp"))
	//{
	////	std::cin.getline(buf, sizeof(buf));
	//	while (std::cin >> buf)
	//	{
	//		if(client.send(buf, strlen(buf)) && client.receive(rcdata))
	//		{
	//			std::string str(rcdata.data(), rcdata.size());
	//			std::cout << ">>>>" << str.data() << std::endl;
	//		}
	//		else
	//			break;
	//	}
	//	client.close();
	//}

	//addr.S_un.S_addr = inet_addr("192.168.1.51");
	//servent *pscse = getservbyname("daytime", "tcp");
	//if(pscse)
	//{
	//	std::cout << "---- servent ------------------------------------" << std::endl;
	//	std::cout << "name:" << pscse->s_name << std::endl 
	//	<< "port:" << pscse->s_port << std::endl
	//	<< "proto:" << pscse->s_proto << std::endl;
	//}
	//hostent *psche = gethostbyname("localhost");
	//if(psche)
	//{
	//	std::cout << "---- hostent ------------------------------------" << std::endl;
	//	std::cout << "name:" << psche->h_name << std::endl
	//	<< "length:" << psche->h_length << std::endl;
	//}
	client.clientHeartBeat("192.168.0.111", 6789);
	std::cout << "clean up socket" << std::endl;
	//acg::socket::clean();
	int over;
	std::cin >> over;
	return 0;
}

