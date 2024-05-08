#pragma once

#include <variant>
#include <vector>
#include <map>
#include <optional>
#include <string>
#include "settingRule.h"
#include "settingTypes.h"
#include "settingInterface.h"
#include "settingLayer.h"
#include "setting.h"

class Setting;

class ISettingStorage {
public:
    ISettingStorage(const std::string &name) : m_alias(name) {};
    virtual bool handle_args(std::vector<std::pair<std::string, bool>> &args, Log &log) { return true; };

    virtual std::map<std::string, std::pair<SettingLayer*, std::optional<std::string>>> GetSettings() = 0;
    void registerLayers(std::vector<SettingLayer*> layers) { m_layers = layers; };
    const std::string Alias() { return m_alias; };
    virtual const std::string LayerAlias(SettingLayer* layer) = 0;

    // Cache operations
    void Push(std::string name, std::optional<std::string> value) { m_cache[name] = value; };
    void Clear() { m_cache.clear(); };
    virtual void Flush(SettingLayer* layer) = 0;

protected:
    std::vector<SettingLayer*> m_layers{};
    std::map<std::string, std::optional<std::string>> m_cache{};

private:
    std::string m_alias;
};

class ISettingInitializer {
public:
    ISettingInitializer(std::vector<SettingInterface*> ifaces, std::vector<ISettingStorage*> storages) : m_interfaces(ifaces), m_storages(storages) {};
    virtual std::vector<Setting> InitializeSettings() = 0;
    std::vector<ISettingStorage*> Storages() { return m_storages; };
    std::vector<SettingInterface*> Interfaces() { return m_interfaces; };
protected:
    std::vector<ISettingStorage*> m_storages;
    std::vector<SettingInterface*> m_interfaces;
};

class ISettingReader {
public:
    ISettingReader(std::string name) : m_name(name) {};
    std::string Alias() { return m_name; };
    virtual std::map<std::string, std::optional<std::string>> GetSettings() = 0;
    virtual void write(std::map<std::string, std::optional<std::string>> &settings) = 0;
private:
    std::string m_name;
};
