// testBC.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>

int main()
{
	std::cout << "系统准备开始测试 .............................................." << std::endl;
	int32_t val = 0x7fffffff;
	float fv = -0.00001;
	std::cout << "强制类型转换0x7fffffff： char=" << std::hex << (char)val << "  byte=" << (uint8_t)val
		<< "   i16=" << (int16_t)val << "    ui16=" << (uint16_t)val
		<< "   ui32=" << (uint32_t)val << "   ui64=" << (uint64_t)val << std::endl;
	std::cout << "浮点0定义 0.00001=" << GTC_FLOAT_IS_ZERO(0.00001)
		<< "   -0.00001=" << GTC_FLOAT_IS_ZERO(fv) << std::endl;
	fv = -0.0001;
	std::cout << "浮点0定义 0.0001=" << GTC_FLOAT_IS_ZERO(0.0001)
		<< "   -0.0001=" << GTC_FLOAT_IS_ZERO(fv) << std::endl;
	fv = -0.0002;
	std::cout << "浮点0定义 0.0002=" << GTC_FLOAT_IS_ZERO(0.0002)
		<< "   -0.0002=" << GTC_FLOAT_IS_ZERO(fv) << std::endl;

	std::cout << "浮点0定义 5=5 :" << GTC_FLOAT_EQUALS(5, 5.0) << std::endl;
	std::cout << "浮点0定义 5=5.0001 :" << GTC_FLOAT_EQUALS(5, 5.0001) << std::endl;
	std::cout << "浮点0定义 5=5.00001 :" << GTC_FLOAT_EQUALS(5, 5.00001) << std::endl;
	std::cout << "浮点0定义 5=5.0002 :" << GTC_FLOAT_EQUALS(5, 5.0002) << std::endl;
	std::cout << "浮点0定义 -5=-5 :" << GTC_FLOAT_EQUALS(-5, -5.0) << std::endl;
	std::cout << "浮点0定义 -5=-5.0001 :" << GTC_FLOAT_EQUALS(-5, -5.0001) << std::endl;
	std::cout << "浮点0定义 -5=-5.00001 :" << GTC_FLOAT_EQUALS(-5, -5.00001) << std::endl;
	std::cout << "浮点0定义 -5=-5.0002 :" << GTC_FLOAT_EQUALS(-5, -5.0002) << std::endl;
	std::cout << std::endl;

	SBDataType sbdata(4);
	int32_t v32 = 0x01020304;
	memcpy_s(sbdata.data(), 4, &v32, 4);
	v32 = 0;
	memcpy_s(&v32, 4, sbdata.data(), 4);

	fv = 123.456;
	//sbdata.clear();
	memcpy_s(sbdata.data(), 4, &fv, 4);
	fv = 0;
	memcpy_s(&fv, 4, sbdata.data(), 4);

	testString();

	//testBitwise();
	//
	//testMemory();

	//testApp();

	testObject();

	//testThread();

	std::cout << "系统测试结束 .............................................." << std::endl;
	//////////////////////////////////////////////////////////////////////////////////////////
	// 2016-1-28		kp007	
	int iIn = 0;
	std::cin >> iIn;
    return 0;
}

