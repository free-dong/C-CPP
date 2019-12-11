
#ifndef _REDIS_H_

#define _REDIS_H_
//
//#include <string.h>
#include <string>
//#include <stdio.h>
#include <hiredis.h>
#include <vector>


typedef struct Redis
{
public:
	Redis() {}
	~Redis()
	{
		this->_connect = NULL;
		this->_reply = NULL;
	}
	bool connect(std::string host, int port)
	{
		this->_connect = redisConnect(host.c_str(), port);
		if (this->_connect != NULL && this->_connect->err)
		{
			printf("connect error: %s\n", this->_connect->errstr);
			return 0;
		}
		return 1;
	}
	std::string get(std::string key)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "GET %s", key.c_str());
		std::string str = this->_reply->str;
		freeReplyObject(this->_reply);
		return str;
	}
	void set(std::string key, std::string value)
	{
		redisCommand(this->_connect, "SET %s %s", key.c_str(), value.c_str());
	}
	void set(std::string key, std::vector<uchar> value)
	{
		redisCommand(this->_connect, "SET %s %b", key.c_str(), value.data(), value.size());
	}
	void sadd(std::string key, std::vector<uchar> value)
	{
		redisCommand(this->_connect, "SADD %s %b", key.c_str(), value.data(), value.size());
	}

	void expire(std::string key, int s)
	{
		redisCommand(this->_connect, "EXPIRE %s %d", key.c_str(), s);
	}

	std::vector<std::string> smembers(std::string key)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "SMEMBERS %s", key.c_str());

		std::vector<std::string> elements;

		for (int i = 0; i < this->_reply->elements; i++)
			elements.push_back(this->_reply->element[i]->str);
		freeReplyObject(this->_reply);
		return elements;
	}
private:
	redisContext* _connect;
	redisReply* _reply;
}Redis;



#endif  //_REDIS_H_
