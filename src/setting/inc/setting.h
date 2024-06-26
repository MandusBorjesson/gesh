#pragma once

#include "log.h"
#include "settingSource.h"
#include "settingRule.h"
#include "settingTypes.h"
#include <string>
#include <map>
#include <vector>
#include <ostream>
#include <optional>

class ISettingRule;
class ISettingStorage;
class ISettingInitializer;
class ISettingReader;

class Setting {
public:
    Setting() = default;
    Setting(const std::string &name,
            ISettingRule *rule,
            ISettingStorage *storage,
            std::vector<SettingInterface*> readers,
            std::vector<SettingInterface*> writers
            );
    bool Set(const std::optional<setting_t> &value, SettingLayer *layer);
    setting_t Get() const;
    std::string Name() const { return m_name; };
    ISettingStorage* Storage() const { return m_storage; };
    SettingLayer* Layer() const { return m_layer; };
    ISettingRule* Rule() const {return m_rule;};
    bool canRead(SettingInterface *iface);
    bool canWrite(SettingInterface *iface);

private:
    std::string m_name;
    setting_t m_value;
    SettingLayer *m_layer{nullptr};
    ISettingStorage *m_storage;
    ISettingRule *m_rule;
    std::vector<SettingInterface*> m_readers;
    std::vector<SettingInterface*> m_writers;
    Log log;
};

class SettingHandler {
public:
    SettingHandler(ISettingInitializer &initializer,
                   Log &logger);

    void importFromReader(std::shared_ptr<ISettingReader> reader, SettingLayer *layer);
    void readSettings(ISettingStorage* storage);
    setting_t Get(const std::string &key, SettingInterface *iface);
    void Set(const std::map<std::string, setting_t> &settings, SettingInterface *iface, SettingLayer *layer);
    std::map<std::string, Setting> GetAll(SettingInterface *iface) const;
    void printSettings();

private:
    void _prettyPrint(const std::map<std::string, Setting> &updated);
    void _handleUpdatedSettings(const std::map<std::string, setting_t> &updated);
    std::map<std::string, Setting> m_settings;
    std::vector<SettingInterface*> m_interfaces;
    std::vector<ISettingStorage*> m_storages;
    Log log;
};
