#include "setting.h"

Setting::Setting(const std::string &name,
                 const std::optional<std::string> &value,
                 std::shared_ptr<ISettingSource> source,
                 ISettingRule *rule,
                 std::vector<SettingInterface*> readers,
                 std::vector<SettingInterface*> writers) : m_name(name), m_source(source), m_rule(rule), m_readers(readers), m_writers(writers), log(Log("setting")) {
    if (!m_rule) {
        throw SettingRuleMissingException(name);
    }

    try {
        m_value = m_rule->ToSetting(value);
        m_good = true;
    } catch ( SettingException const& ex ) {
        log.error() << "Failed to initialize setting: " << m_name << ", " << ex.what();
    }
}

bool Setting::Set(const std::optional<setting_t> &value, std::shared_ptr<ISettingSource> source) {
    setting_t new_value;
    try {
        new_value = m_rule->ToSetting(value);
    } catch ( SettingException const& ex ) {
        log.warning() << "Failed to set setting: " << m_name << ", " << ex.what();
        return false;
    }

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

std::ostream& operator<<(std::ostream& os, const Setting& s)
{
    os << "[" << s.Source() << "] " << s.Name() << " ";
    auto val = s.Get();

    if (std::holds_alternative<std::string>(val)) {
        os << std::get<std::string>(val);
    } else if (std::holds_alternative<int>(val)) {
        os << std::get<int>(val);
    } else if (std::holds_alternative<bool>(val)) {
        os << std::get<bool>(val) ? "True" : "False";
    } else if (std::holds_alternative<std::monostate>(val)) {
        os << "<un-initialized>";
    } else {
        os << "N/A";
    }

    return os;
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

    auto settings = initializer.InitializeSettings();
    bool settings_ok = true;

    for (auto setting : settings) {
        if (setting.Ok()) {
            m_settings[setting.Name()] = setting;
        } else {
            settings_ok = false;
        }
    }
    if (!settings_ok) {
        log.alert() << "One or more setting(s) failed initialization. This is bad, and probably programmer error. The settings will not be available!";
    }

    for (auto reader : readers) {
        readSettings(reader);
    }
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
        } catch ( SettingRuleException const& ex ) {
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
