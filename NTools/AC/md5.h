#ifndef __MD5_H__
#define __MD5_H__
#pragma once

#include <cstdint>
#include <string>
#include <istream>
//#include "basedef.h"
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	md5	Àà 2015-12-7	kp007
//	md5Ëã·¨·â×°
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class md5
{
public:
	md5();
	~md5();

	md5(const void *input, std::size_t length);
	md5(const std::string &str);
	md5(std::ifstream &in);
	void update(const void *input, std::size_t length);
	void update(const std::string &str);
	void update(std::ifstream &in);
	const uint8_t* digest();
	std::string toString(bool bUpper = false);
	void reset();
private:  
	void update(const uint8_t *input, std::size_t length);
	void final();
	void transform(const uint8_t block[64]);
	void encode(const uint32_t *input, uint8_t *output, std::size_t length);
	void decode(const uint8_t *input, uint32_t *output, std::size_t length);
	//std::string bytesToHexString(const uint8_t *input, std::size_t length);  

	/* class uncopyable */  
	md5(const md5&); 
	md5& operator=(const md5&);
private:  
	uint32_t	_state[4];		/* state (ABCD) */  
	uint32_t	_count[2];		/* number of bits, modulo 2^64 (low-order word first) */  
	uint8_t		_buffer[64];	/* input buffer */  
	uint8_t		_digest[16];	/* message digest */  
	bool		_finished;		/* calculate finished ? */  

	static const uint8_t		PADDING[64];  /* padding for calculate */  
	static const char			HEX[16];  
	static const std::size_t	BUFFER_SIZE = 1024; 
};
}	// gtc
#endif	// __MD5_H__