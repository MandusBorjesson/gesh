
#include "log.h"
#include "settingReaderFile.h"
#include <fstream>
#include <map>
#include <sstream>


SettingReaderCsv::SettingReaderCsv(const std::string &path) : ISettingReader(path), m_path(path) {
    INFO << "[SettingReaderCsv] Created for path " << path << std::endl;
};

std::map<std::string, std::string> SettingReaderCsv::GetSettings() {
    std::map<std::string, std::string> out;

    std::ifstream infile(m_path);
    if (!infile.is_open()) {
        WARNING << "[SettingReaderCsv] failed to open " << m_path << std::endl;
        return out;
    }

    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (!(iss >> key >> value)) {
            WARNING << "[SettingReaderCsv] Failed to parse line, ignoring: \"" << line << "\""<< std::endl;
            continue;
        }
        out[key] = value;
    }
    return out;
}
