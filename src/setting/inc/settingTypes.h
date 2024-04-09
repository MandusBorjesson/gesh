#pragma once

#include <stdexcept>
#include <sstream>
#include <cstring>
#include <variant>
#include <string>

using setting_t = std::variant<std::monostate, bool, int, std::string>;

struct SettingException : public std::exception {
    SettingException(const std::string &msg, const std::string child = "") : m_exc("setting" + child), m_msg(m_exc + ": " + msg) {}
    virtual const char* what() const throw()
    {
      return m_msg.c_str();
    }
    std::string who() const {
        return m_exc;
    }
protected:
    std::string m_exc;
    std::string m_msg;
};
