#ifndef LOG_H
#define LOG_H

#include <iostream>

namespace {
    enum LogLevel {
        FATAL = 0,
        ALERT,
        CRITICAL,
        ERROR,
        WARNING,
        NOTICE,
        INFO,
        DEBUG,
        NONE,
        SILENT,
    };
}

std::string getColor(LogLevel level) {
    switch(level) {
    case DEBUG:
        return "\033[0;90m";
    case INFO:
        return "\033[0;37m";
    case NOTICE:
        return "\033[0;36m";
    case WARNING:
        return "\033[0;33m";
    case ERROR:
        return "\033[0;91m";
    case CRITICAL:
        return "\033[0;31m";
    case ALERT:
        return "\033[0;101m";
    case FATAL:
        return "\033[0;41m";
    default:
        return "\033[0m";
    }
}

class JournalLog {
public:
    JournalLog(const std::string &name, LogLevel level) : m_muted(level == SILENT){
        if (m_muted) {
            return;
        }
        operator << (getColor(level) + (level == NONE ? "" : "<"+std::to_string(level)+"> ") +
                    (name.empty() ? "" : "["+name+"] "));
    }
    ~JournalLog() {
        if (!m_muted) {
            std::cout << getColor(NONE) << std::endl;
        }
    }
    template<class T>
    JournalLog &operator<<(const T &msg) {
        if (!m_muted) {
            std::cout << msg;
        }
        return *this;
    }
private:
    bool m_muted;
};

class Log {
public:
    Log(): Log("") {};
    Log(std::string name): m_name(std::move(name)) {
        for ( auto & c : m_name ) {
            if ( ! isalnum( c ) ) c = '.';
        }
    };

    void mute(bool mute = true) { m_muted = mute; };
    Log getChild(std::string name) {
        Log child = Log(m_name + "." + name);
        child.mute(m_muted);
        return child;
    };

    JournalLog fatal() const { return JournalLog(m_name, (m_muted ? SILENT : FATAL)); };
    JournalLog alert() const { return JournalLog(m_name, (m_muted ? SILENT : ALERT)); };
    JournalLog critical() const { return JournalLog(m_name, (m_muted ? SILENT : CRITICAL)); };
    JournalLog error() const { return JournalLog(m_name, (m_muted ? SILENT : ERROR)); };
    JournalLog warning() const { return JournalLog(m_name, (m_muted ? SILENT : WARNING)); };
    JournalLog notice() const { return JournalLog(m_name, (m_muted ? SILENT : NOTICE)); };
    JournalLog info() const { return JournalLog(m_name, (m_muted ? SILENT : INFO)); };
    JournalLog debug() const { return JournalLog(m_name, (m_muted ? SILENT : DEBUG)); };
    JournalLog none() const { return JournalLog(m_name, (m_muted ? SILENT : NONE)); };
private:
    std::string m_name;
    bool m_muted{false};
};

#endif  /* LOG_H  */
