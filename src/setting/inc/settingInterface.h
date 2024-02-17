#pragma once

#include <string>
#include <memory>

class ISettingApiManager {
public:
    virtual void handleSettingsUpdated(const std::map<std::string, setting_t>& settings) = 0;
};

class SettingInterface {
public:
    SettingInterface(const std::string name) : m_name(name) {};
    std::string Name() const {return m_name;}
    void RegisterManager(std::shared_ptr<ISettingApiManager> manager) {m_manager = manager;}
    std::shared_ptr<ISettingApiManager> Manager() const {return m_manager;}
private:
    std::string m_name;
    std::shared_ptr<ISettingApiManager> m_manager;
};
