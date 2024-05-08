#include "settingStorageFile.h"
#include "settingReaderFile.h"
#include <algorithm>

bool SettingStorageFile::handle_args(std::vector<std::pair<std::string, bool>> &args, Log &log) {
    bool ret = false;
    for (int i = 0; i < args.size(); i++) {
        if (args[i].first == "-h" || args[i].first == "--help") {
            args[i].second = true;

            log.none() << "Options - File Storage";
            log.none() << " --storage-file-path <path>   (required) Path to where setting file fragments are stored";
            log.none();
            return true;
        }
        if (args[i].first == "--storage-file-path") {
            if (i+1 < args.size()) {
                m_basePath = args[i+1].first;

                // Consume flag and arg, jump ahead
                args[i].second = true;
                args[i+1].second = true;
                i++;
                ret = true;
            }
        }
    }
    return ret;
}

std::map<std::string, std::pair<SettingLayer*, std::optional<std::string>>> SettingStorageFile::GetSettings() {
    std::map<std::string, std::pair<SettingLayer*, std::optional<std::string>>> ret;
    for ( auto layer : m_layers ) {
        std::string name = m_basePath + layer->name() + ".csv";
        log.debug() << "Fetching values from: " << name;
        auto reader = SettingReaderCsv(name, log);
        auto settings = reader.GetSettings();
        for ( auto [name, val] : settings ) {
            ret[name] = std::make_pair(layer, val);
        }
    }
    return ret;
};

void SettingStorageFile::Flush(SettingLayer *layer) {

    if (std::find(m_layers.begin(), m_layers.end(), layer) == m_layers.end()) {
        return;
    }
    if (m_cache.empty()) {
        return;
    }

    std::string name = m_basePath + layer->name() + ".csv";
    log.notice() << "Flushing " << m_cache.size() <<  " settings to " << name;

    auto reader = SettingReaderCsv(name, log);
    auto settings = reader.GetSettings();

    for (auto &[key, value] : m_cache) {
        settings[key] = value;
    }

    reader.write(settings);

    Clear();
};

const std::string SettingStorageFile::LayerAlias(SettingLayer *layer) {
    return m_basePath + layer->name() + ".csv";
}
