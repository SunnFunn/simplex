#ifndef _REDIS_H_
#define _REDIS_H_
#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>

#include </opt/homebrew/Cellar/hiredis/1.3.0/include/hiredis/hiredis.h>
// #include <hiredis/hiredis.h>

class Redis{
public:
    Redis(){};

    ~Redis(){
        this->_connect = nullptr;
        this->_reply = nullptr;
    }

    bool  connect(std::string host, int port){
        this->_connect = redisConnect(host.c_str(), port);
        if(this->_connect != nullptr && this->_connect->err){
            printf("connect error: %s\n", this->_connect->errstr);
            return false;
        }

        return true; //1
    }


    std::string get(const std::string& key, const std::string& password)
    {
        redisCommand(this->_connect, "AUTH %s", password.c_str());
        this->_reply = (redisReply*)redisCommand(this->_connect, "GET %s", key.c_str());
        std::string str = this->_reply->str;
        freeReplyObject(this->_reply);
        return str;
    }


    void set(const std::string& key, const std::string& value, const std::string& password)
    {
        redisCommand(this->_connect, "AUTH %s", password.c_str());
        redisCommand(this->_connect, "SET %s %s", key.c_str(), value.c_str());
    }
private:
    redisContext* _connect;
    redisReply* _reply;
};

#endif