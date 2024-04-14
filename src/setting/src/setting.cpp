#include "setting.h"
#include <iomanip>
#include <iostream>

std::string toString(const setting_t &setting) {
    if (std::holds_alternative<std::string>(setting)) {
        return std::get<std::string>(setting);
    }
    if (std::holds_alternative<int>(setting)) {
        return std::to_string(std::get<int>(setting));
    }
    if (std::holds_alternative<bool>(setting)) {
        return std::get<bool>(setting) ? "True" : "False";
    }
    return "-";
}

Setting::Setting(const std::string &name,
                 std::shared_ptr<ISettingSource> source,
                 ISettingRule *rule,
                 std::vector<SettingInterface*> readers,
                 std::vector<SettingInterface*> writers) : m_name(name), m_source(source), m_rule(rule), m_readers(readers), m_writers(writers) {
    if (!m_rule) {
        throw SettingRuleMissingException(name);
    }
}

bool Setting::Set(const std::optional<setting_t> &value, std::shared_ptr<ISettingSource> source) {
    setting_t new_value = m_rule->ToSetting(value);

    m_source = source;

    if (m_value == new_value) {
        return false;
    }
    m_value = new_value;
    return true;
}

setting_t Setting::Get() const {
    return m_value;
}

std::string Setting::Source() const {
    if (m_source) {
        return m_source->Alias();
    } else {
        return "None";
    }
}

bool Setting::canRead(SettingInterface *iface) {
    // nullptr is used to indicate internal access
    if (!iface) {
        return true;
    }
    for (auto interface : m_readers) {
        if (iface == interface) {
            return true;
        }
    }
    return false;
}

bool Setting::canWrite(SettingInterface *iface) {
    for (auto interface : m_writers) {
        if (iface == interface) {
            return true;
        }
    }
    return false;
}

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

SettingHandler::SettingHandler(ISettingInitializer &initializer,
                               std::vector<std::shared_ptr<ISettingReader>> &readers,
                               Log &logger): m_interfaces(initializer.Interfaces()), log(logger.getChild("handler")) {
    log.notice() << "Settings handler instantiated, initializing settings... ";

    auto settings = initializer.InitializeSettings();
    for (auto setting : settings) {
        m_settings[setting.Name()] = setting;
    }

    for (auto reader : readers) {
        readSettings(reader);
    }

    log.notice() << "Setting initialization DONE. ";
    _prettyPrint(m_settings);
}

setting_t SettingHandler::Get(const std::string &key, SettingInterface *iface) {
    if (m_settings.find(key) == m_settings.end()) {
        throw SettingKeyException(key);
    }
    if (!m_settings[key].canRead(iface)) {
        throw SettingAccessException(key);
    }
    auto val = m_settings[key].Get();
    if (std::holds_alternative<std::monostate>(val)) {
        throw SettingNoValueException(key);
    }
    return val;
}

void SettingHandler::_prettyPrint(const std::map<std::string, Setting> &updated) {
    constexpr auto W_KEY = 25;
    constexpr auto W_VAL = 20;
    constexpr auto W_IFC = 10;

    log.info() << std::left << std::setw(W_KEY) << "Key"
               << std::setw(W_VAL) << "Value"
               << std::setw(W_IFC*m_interfaces.size()) << "Permissions"
               << "  " << "Source";
    auto os = log.info();

    os << std::left << std::setw(W_KEY + W_VAL) << "";
    for (const auto interface : m_interfaces) {
        os << std::setw(W_IFC) << interface->Name();
    }
    os << "\n";

    for ( auto const& [key, val] : m_settings ) {

        auto os = log.info();
        os << std::left;
        os << std::setw(W_KEY) << key
           << std::setw(W_VAL) << toString(val.Get());

        for (const auto interface : m_interfaces) {
            std::string perm;
            perm.append(m_settings[key].canRead(interface) ? "r" : "-");
            perm.append(m_settings[key].canWrite(interface) ? "w" : "-");
            os << std::setw(W_IFC) << perm;
        }
        os << "  " << val.Source();
    }
}

void SettingHandler::_handleUpdatedSettings(const std::map<std::string, setting_t> &updated) {
    log.info() << updated.size() << " settings updated";
    for (const auto interface : m_interfaces) {
        std::map<std::string, setting_t> temp;
        for (const auto& [key, val] : updated) {
            if(m_settings[key].canRead(interface)) {
                temp[key] = val;
            }
        }
        auto manager = interface->Manager();
        if (manager) {
            manager->handleSettingsUpdated(temp);
        } else {
            log.debug() << "Interface '" << interface->Name() << "' has no manager, skipping";
        }
    }
}

void SettingHandler::readSettings(std::shared_ptr<ISettingReader> reader) {
    log.info() << "Fetching settings from " << reader->Alias();
    std::map<std::string, setting_t> updated;
    for (auto setting : reader->GetSettings()) {
        if (m_settings.find(setting.first) == m_settings.end()) {
            log.warning() << "Unknown setting: " << setting.first;
            continue;
        }
        try {
            auto val = m_settings[setting.first].Rule()->ToSetting(setting.second);
            if(m_settings[setting.first].Set(val, reader)) {
                updated[setting.first] = val;
            }
        } catch ( SettingException const& ex ) {
            log.warning() << "Failed to set setting: " << setting.first << ", " << ex.what();
            continue;
        }
        log.debug() << "Value of " << setting.first << " updated to " << setting.second.value_or("<un-initialized>");
    }
    _handleUpdatedSettings(updated);
}

void SettingHandler::Set(const std::map<std::string, setting_t> &settings, SettingInterface *iface) {
    for ( auto const& [key, val] : settings ) {
        if (m_settings.find(key) == m_settings.end()) {
            throw SettingKeyException(key);
        }
        if (!m_settings[key].canWrite(iface)) {
            throw SettingAccessException(key);
        }

        // Will throw if validation fails
        m_settings[key].Rule()->ToSetting(val);
    }
    std::map<std::string, setting_t> updated;
    for ( auto const& [key, val] : settings ) {
        // The above call to 'ToSetting' should ensure no exceptions are thrown here.
        if(m_settings[key].Set(val, nullptr)) {
            updated[key] = val;
        }
    }

    _handleUpdatedSettings(updated);
}

std::map<std::string, Setting> SettingHandler::GetAll(SettingInterface *iface) const {
    std::map<std::string, Setting> ret;
    for ( auto entry : m_settings ) {
        if(entry.second.canRead(iface)) {
            ret[entry.first] = entry.second;
        }
    }
    return ret;
}
