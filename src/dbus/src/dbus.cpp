#include "dbus.h"
#include "settingReaderFactory.h"

dbusGet_t DBusGeshSetting::Get(const std::vector<std::string>& keys) {
    log.info() << "Get called, " << keys.size() << " settings requested";

    dbusGet_t ret;
    for (auto key: keys) {
        try {
            auto setting = m_handler->Get(key, m_iface);
            std::get<0>(ret)[key] = sdbus::Variant(ToSdBusVariant(setting));
        } catch ( SettingException const & ex ) {
            log.warning() << "Failed to get: " << key << ", " << ex.what();
            std::get<1>(ret)[key] = ex.who();
        }
    }
    log.info()  << "Get returned " << std::get<0>(ret).size() << " settings, " << std::get<1>(ret).size()<< " errors";
    return ret;
}

std::tuple<std::map<std::string, sdbus::Variant>, std::vector<std::string>> DBusGeshSetting::GetAll() {
    std::tuple<std::map<std::string, sdbus::Variant>, std::vector<std::string>> out;
    for (auto setting: m_handler->GetAll(m_iface)) {
        auto val = setting.second.Get();
        if (std::holds_alternative<std::monostate>(val)) {
            std::get<1>(out).push_back(setting.first);
        } else {
            std::get<0>(out)[setting.first] = ToSdBusVariant(val);
        }
    }
    return out;
}

void DBusGeshSetting::Set(const std::string &layer, const std::map<std::string, sdbus::Variant>& update, const std::vector<std::string>& invalidate) {
    log.notice() << "Set called";

    std::map<std::string, setting_t> in;
    for (auto const& [key, val] : update) {
        in[key] = ToSetting(val);
    }

    for (auto const& key : invalidate) {
        in[key] = setting_t();
    }

    try {
        m_handler->Set(in, m_iface, m_layerHandler->findLayer(layer));
    } catch ( SettingException const & ex ) {
        log.warning() << "Set failed: " << ex.what();
        throw sdbus::Error("own.gesh.Error", ex.what());
    }

}

sdbus::Variant DBusGeshSetting::ToSdBusVariant(const setting_t &val) const {
    if (std::holds_alternative<std::string>(val)) {
        return sdbus::Variant(std::get<std::string>(val));
    } else if (std::holds_alternative<int>(val)) {
        return sdbus::Variant(std::get<int>(val));
    } else if (std::holds_alternative<bool>(val)) {
        return sdbus::Variant(std::get<bool>(val));
    } else {
        return sdbus::Variant(NULL);
    }
}
setting_t DBusGeshSetting::ToSetting(const sdbus::Variant &val) const {
    auto type = val.peekValueType();
    if (type == "s") {
        return std::string(val);
    } else if (type == "i") {
        return int(val);
    } else if (type == "b") {
        return bool(val);
    } else {
        auto err = "Unknown variant type '" + type + "'";
        throw sdbus::Error("own.gesh.Error", err);
    }
}

void DBusGeshSetting::handleSettingsUpdated(const std::map<std::string, setting_t>& settings) {
    std::map<std::string, sdbus::Variant> out;
    std::vector<std::string> invalidated;
    for (auto const& [key, val] : settings) {
        if (std::holds_alternative<std::monostate>(val)) {
            invalidated.push_back(key);
        } else {
            out[key] = ToSdBusVariant(val);
        }
    }

    if (!out.empty() || !invalidated.empty()) {
        log.info() << "Emitting SettingsUpdated: " << out.size() << " changed, " << invalidated.size() << " invalidated";
        emitSettingsUpdated(out, invalidated);
    } else {
        log.debug() << "No setting changed, not emitting SettingsUpdated";
    }
}

void DBusGeshManagement::Import(const std::string& layer, const std::string& file) {
    log.info() << "Import called: " << file;
    try {
        m_handler->importFromReader(Factory::Reader(file), m_layerHandler->findLayer(layer));
    } catch ( SettingException const & ex ) {
        log.warning() << "Import failed: " << ex.what();
        throw sdbus::Error("own.gesh.Error", ex.what());
    }
}
