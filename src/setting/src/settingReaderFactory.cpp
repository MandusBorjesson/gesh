#include "log.h"
#include "settingReaderFactory.h"
#include "settingReaderFile.h"
#include <filesystem>

std::vector<ISettingReader*> SettingReaderFactory::getReaders() {
    INFO << "[SettingReaderFactory] Finding setting readers..." << std::endl;
    DEBUG << "[SettingReaderFactory] Search paths:" << std::endl;
    for ( auto const &p : m_paths ) {
        DEBUG << "[SettingReaderFactory]   " << p << std::endl;
    }

    std::vector<ISettingReader*> out;
    for (auto const &path : m_paths) {
        for (const auto & file : std::filesystem::directory_iterator(path)) {
            std::string p = file.path();
            auto suffix = p.substr(p.find_last_of(".") + 1);
            if ( suffix == "csv" ) {
                out.push_back(new SettingReaderCsv(p));
            }
        }
    }

    return out;
}
