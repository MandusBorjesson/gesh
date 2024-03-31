#ifndef LOG_H
#define LOG_H

#include <iostream>

namespace {
    enum LogLevel {
        DEBUG = 0,
        INFO,
        NOTICE,
        WARNING,
        ERROR,
        CRITICAL,
        ALERT,
        FATAL,
        NONE,
    };
}

class JournalLog {
public:
    JournalLog(std::string &name, LogLevel level) {
        operator << ((level == NONE ? "" : "<"+std::to_string(level)+"> ") +
                    (name.empty() ? "" : "["+name+"] "));
    }
    ~JournalLog() {
        std::cout << std::endl;
    }
    template<class T>
    JournalLog &operator<<(const T &msg) {
        std::cout << msg;
        return *this;
    }
};

class Log {
public:
    Log(): m_name("") {};
    Log(std::string name): m_name(std::move(name)) {
        for ( auto & c : m_name ) {
            if ( ! isalnum( c ) ) c = '.';
        }
    };

    Log getChild(std::string name)  { return Log(m_name + "." + name); };

    JournalLog fatal()    { return JournalLog(m_name, FATAL); };
    JournalLog alert()    { return JournalLog(m_name, ALERT); };
    JournalLog critical() { return JournalLog(m_name, CRITICAL); };
    JournalLog error()    { return JournalLog(m_name, ERROR); };
    JournalLog warning()  { return JournalLog(m_name, WARNING); };
    JournalLog notice()   { return JournalLog(m_name, NOTICE); };
    JournalLog info()     { return JournalLog(m_name, INFO); };
    JournalLog debug()    { return JournalLog(m_name, DEBUG); };
    JournalLog none()     { return JournalLog(m_name, NONE); };
private:
    std::string m_name;
};

#endif  /* LOG_H  */
