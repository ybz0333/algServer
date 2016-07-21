/* 
简单的日志记录类. (日志而已，何必那么复杂！！！） 
W.J.Chang 2013.12.13 
 
说明:(EasyLog.h) 
1, 简单的单件实现（自动垃圾回收） 
2, 使用方法：EasyLog::Inst()->Log("Run..."); 
3, 日志记录结果：Run...    [2013.12.13 16:38:42 Friday] 
*/  
#pragma once  
#ifndef EasyLog_H 
#define EasyLog_H 
#include <memory>  
#include <ctime>  
#include <iostream>  
#include <fstream>  
class EasyLog  
{  
public:  
    static EasyLog * Inst(){  
        if (0 == _instance.get()){  
            _instance.reset(new EasyLog);  
        }  
        return _instance.get();  
    }  
  
    void Log(std::string msg); // 写日志的方法  
private:  
    EasyLog(void){}  
    virtual ~EasyLog(void){}  
    friend class std::auto_ptr<EasyLog>;  
    static std::auto_ptr<EasyLog> _instance;  
};  
  
std::auto_ptr<EasyLog> EasyLog::_instance;  
  
void EasyLog::Log(std::string loginfo) {  
    std::ofstream ofs;  
    time_t t = time(0);  
    char tmp[64];  
    strftime(tmp, sizeof(tmp), "\t[%Y.%m.%d %X %A]", localtime(&t));  
    ofs.open("Dynami.log", std::ofstream::app);  
    ofs.write(loginfo.c_str(), loginfo.size());  
    ofs << tmp << '\n';  
    ofs.close();  
}  
#endif  