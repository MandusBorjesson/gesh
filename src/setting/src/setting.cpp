#include "setting.h"
#include <iomanip>
#include <iostream>
#include <limits>

std::optional<std::string> toString(const setting_t &setting) {
    if (std::holds_alternative<std::string>(setting)) {
        return std::get<std::string>(setting);
    }
    if (std::holds_alternative<int>(setting)) {
        return std::to_string(std::get<int>(setting));
    }
    if (std::holds_alternative<bool>(setting)) {
        return std::get<bool>(setting) ? "True" : "False";
    }
    return std::nullopt;
}

Setting::Setting(const std::string &name,
                 ISettingRule *rule,
                 ISettingStorage *storage,
                 std::vector<SettingInterface*> readers,
                 std::vector<SettingInterface*> writers,
                 std::string gatekeeper) : m_name(name), m_rule(rule), m_storage(storage), m_readers(readers), m_writers(writers), m_gatekeeper(gatekeeper) {
    if (!m_rule) {
        throw SettingRuleMissingException(name);
    }
    if (!m_storage) {
        throw SettingException(name);
    }
}

bool Setting::Set(const std::optional<setting_t> &value, SettingLayer *layer) {
    setting_t new_value = m_rule->ToSetting(value);

    m_storage->Push(m_name, toString(new_value));

    int old_prio = m_layer ? m_layer->priority() : std::numeric_limits<int>::min();
    int new_prio = layer ? layer->priority() : std::numeric_limits<int>::min();
    if (old_prio > new_prio) {
        return false;
    }
    m_layer = layer;

    if (m_value == new_value) {
        return false;
    }
    m_value = new_value;
    return true;
}

settingStatus Setting::Get(SettingInterface *iface, setting_t* out) const {
    if (iface && !canRead(iface)) {
        return ERROR_READ;
    }
    *out = m_value;
    return OK;
}

bool Setting::canRead(SettingInterface *iface) const {
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

bool Setting::canWrite(SettingInterface *iface) const {
    for (auto interface : m_writers) {
        if (iface == interface) {
            return true;
        }
    }
    return false;
}

SettingHandler::SettingHandler(ISettingInitializer &initializer,
                               Log &logger): m_interfaces(initializer.Interfaces()), m_storages(initializer.Storages()), log(logger.getChild("handler")) {
    log.notice() << "Settings handler instantiated, initializing settings... ";

    auto settings = initializer.InitializeSettings();
    for (auto setting : settings) {
        m_settings[setting.Name()] = setting;
    }

    for (auto storage : m_storages) {
        readSettings(storage);
    }
}

void SettingHandler::printSettings() {
    _prettyPrint(m_settings);
}

setting_t SettingHandler::Get(const std::string &key, SettingInterface *iface) {
    if (m_settings.find(key) == m_settings.end()) {
        throw SettingKeyException(key);
    }
    setting_t val;
    switch(m_settings[key].Get(iface, &val)) {
        case OK:
            break;
        case ERROR_READ:
            throw SettingAccessException(key);
    }
    if (!_isEnabled(m_settings[key])) {
        throw SettingDisabledException(key);
    }
    if (std::holds_alternative<std::monostate>(val)) {
        throw SettingNoValueException(key);
    }
    return val;
}

void SettingHandler::_prettyPrint(const std::map<std::string, Setting> &updated) {
    constexpr auto W_KEY = 25;
    constexpr auto W_VAL = 20;
    constexpr auto W_IFC = 10;
    constexpr auto W_STO = 10;
    constexpr auto W_LAY = 10;

    log.info() << std::left << std::setw(W_KEY) << "Key"
               << std::setw(W_VAL) << "Value"
               << std::setw(W_IFC*m_interfaces.size()) << "Permissions"
               << std::setw(W_STO) << "Storage"
               << std::setw(W_LAY) << "Layer"
               << "  " << "Source";
    auto os = log.info();

    os << std::left << std::setw(W_KEY + W_VAL) << "";
    for (const auto interface : m_interfaces) {
        os << std::setw(W_IFC) << interface->Name();
    }
    os << "\n";

    for ( auto const& [key, val] : m_settings ) {

        auto os = log.info();
        setting_t setting_value;
        // Errors here are fine, setting_value will simply not be updated
        std::ignore = val.Get(nullptr, &setting_value);
        auto val_str = toString(setting_value);
        os << std::left;
        os << std::setw(W_KEY) << key
           << std::setw(W_VAL) << val_str.value_or("-");

        for (const auto interface : m_interfaces) {
            std::string perm;
            perm.append(m_settings[key].canRead(interface) ? "r" : "-");
            perm.append(m_settings[key].canWrite(interface) ? "w" : "-");
            os << std::setw(W_IFC) << perm;
        }
        os << std::setw(W_STO) << val.Storage()->Alias();
        if (auto lay = val.Layer()) {
            os << std::setw(W_LAY) << lay->name();

            // Don't print source for the default layer. Coincidentally, this
            // is the only layer with prio lower than 0.
            if (lay->priority() < 0) {
                os << "  -";
            } else {
                os << "  " << val.Storage()->LayerAlias(lay);
            }
        } else {
            os << std::setw(W_LAY) << "-";
            os << "  -";
        }
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

bool SettingHandler::_isEnabled(const Setting &setting) const {
    std::string gatekeeper_key = setting.Gatekeeper();
    if (gatekeeper_key == "") {
        return true;
    }
    if (m_settings.find(gatekeeper_key) == m_settings.end()) {
        return false;
    }
    setting_t val;
    // If this fails for any reason, val should remain monostate and we will return false
    std::ignore = m_settings.at(gatekeeper_key).Get(nullptr, &val);

    if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val);
    } else {
        return false;
    }
}

void SettingHandler::importFromReader(std::shared_ptr<ISettingReader> reader, SettingLayer *layer) {
    if (!reader) {
        throw SettingException("No valid reader provided");
    }

    if (!layer) {
        throw SettingException("No valid layer provided");
    }

    log.info() << "Getting settings from " << reader->Alias();
    std::map<std::string, setting_t> updated;
    auto settings = reader->GetSettings();
    log.info() << settings.size() << " settings fetched, updating internal values...";

    for (auto element : settings) {
        auto key = element.first;
        auto value = element.second;
        if (m_settings.find(key) == m_settings.end()) {
            log.warning() << "Unknown setting: " << key;
            continue;
        }

        try {
            auto val = m_settings[key].Rule()->ToSetting(value);
            if(m_settings[key].Set(val, layer)) {
                updated[key] = val;
                log.debug() << "Value of " << key << " updated to " << value.value_or("<un-initialized>");
            }
        } catch ( SettingException const& ex ) {
            log.warning() << "Failed to set setting: " << key << ", " << ex.what();
            continue;
        }
    }
    _handleUpdatedSettings(updated);
}

void SettingHandler::readSettings(ISettingStorage* storage) {
    log.info() << "Fetching settings from " << storage->Alias();
    std::map<std::string, setting_t> updated;
    auto settings = storage->GetSettings();
    log.info() << settings.size() << " settings fetched, updating internal values...";

    for (auto element : settings) {
        auto key = element.first;
        auto layer = element.second.first;
        auto value = element.second.second;
        if (m_settings.find(key) == m_settings.end()) {
            log.warning() << "Unknown setting: " << key;
            continue;
        }

        if (storage != m_settings[key].Storage()) {
            log.warning() << "Source pollution detected, ignoring: " << key;
            continue;
        }

        try {
            auto val = m_settings[key].Rule()->ToSetting(value);
            if(m_settings[key].Set(val, layer)) {
                updated[key] = val;
            }
        } catch ( SettingException const& ex ) {
            log.warning() << "Failed to set setting: " << key << ", " << ex.what();
            continue;
        }
        log.debug() << "Value of " << key << " updated to " << value.value_or("<un-initialized>");
    }
    _handleUpdatedSettings(updated);
}


void SettingHandler::Set(const std::map<std::string, setting_t> &settings, SettingInterface *iface, SettingLayer *layer) {
    if (!layer) {
        throw SettingException("No valid layer provided");
    }
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
    for ( auto const& storage : m_storages ) {
        storage->Clear();
    }

    std::map<std::string, setting_t> updated;
    for ( auto const& [key, val] : settings ) {
        // The above call to 'ToSetting' should ensure no exceptions are thrown here.
        if(!m_settings[key].Set(val, layer)) {
            continue;
        }
        if(!_isEnabled(m_settings[key])) {
            continue;
        }
        updated[key] = val;
    }

    for ( auto const& storage : m_storages ) {
        storage->Flush(layer);
    }

    _handleUpdatedSettings(updated);
}

std::map<std::string, Setting> SettingHandler::GetAll(SettingInterface *iface) const {
    std::map<std::string, Setting> ret;
    for ( auto entry : m_settings ) {
        if(!_isEnabled(entry.second)) {
            continue;
        }
        ret[entry.first] = entry.second;
    }
    return ret;
}
