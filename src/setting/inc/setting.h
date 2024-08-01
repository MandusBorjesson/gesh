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

enum settingStatus {
    OK,
    ERROR_READ,
};

class ISettingRule;
class ISettingStorage;
class ISettingInitializer;
class ISettingReader;

class SettingHandlerException : public SettingException {
public:
    SettingHandlerException(const std::string &msg) : SettingException(msg, ".handler") {}
};

class SettingKeyException : public SettingException {
public:
    SettingKeyException(const std::string &msg) : SettingException(msg, ".key") {}
};

class SettingAccessException : public SettingException {
public:
    SettingAccessException(const std::string &msg) : SettingException(msg, ".access") {}
};

class SettingNoValueException : public SettingException {
public:
    SettingNoValueException(const std::string &msg) : SettingException(msg, ".novalue") {}
};

class SettingDisabledException : public SettingException {
public:
    SettingDisabledException(const std::string &msg) : SettingException(msg, ".disabled") {}
};

class Setting {
public:
    Setting() = default;
    Setting(const std::string &name,
            ISettingRule *rule,
            ISettingStorage *storage,
            std::vector<SettingInterface*> readers,
            std::vector<SettingInterface*> writers,
            std::string gatekeeper = "");
    bool Set(const std::optional<setting_t> &value, SettingLayer *layer);
    /**
     * \brief Get the value contained in the setting
     * \param iface the interface requesting the value.
     * \param out if the function returns OK, the value will be stored in this
     *        pointer. In all other cases, it will be unchanged.
     * \return OK if get request was successful.
     */
    [[nodiscard]] settingStatus Get(SettingInterface *iface, setting_t* out) const;
    std::string Name() const { return m_name; };
    std::string Gatekeeper() const { return m_gatekeeper; };
    ISettingStorage* Storage() const { return m_storage; };
    SettingLayer* Layer() const { return m_layer; };
    ISettingRule* Rule() const {return m_rule;};
    bool canRead(SettingInterface *iface) const;
    bool canWrite(SettingInterface *iface) const;

private:
    std::string m_name;
    setting_t m_value;
    SettingLayer *m_layer{nullptr};
    ISettingStorage *m_storage;
    ISettingRule *m_rule;
    std::vector<SettingInterface*> m_readers;
    std::vector<SettingInterface*> m_writers;
    std::string m_gatekeeper;
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
    bool _isEnabled(const Setting &setting) const;
    std::map<std::string, Setting> m_settings;
    std::vector<SettingInterface*> m_interfaces;
    std::vector<ISettingStorage*> m_storages;
    Log log;
};
