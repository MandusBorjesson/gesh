#pragma once

#include <stdexcept>
#include <sstream>
#include <cstring>
#include <variant>
#include <string>

using setting_t = std::variant<std::monostate, bool, int, std::string>;

struct SettingException : public std::exception {
    SettingException(std::string& msg) : m_msg(msg) {}
    virtual const char* what() const throw()
    {
      return m_msg.c_str();
    }
protected:
    std::string m_msg;
};
