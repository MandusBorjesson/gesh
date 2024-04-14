
#include "settingReaderFile.h"
#include <fstream>
#include <map>
#include <sstream>


SettingReaderCsv::SettingReaderCsv(const std::string &path, Log &logger) : ISettingReader(path), m_path(path), log(logger.getChild("file")) {};

std::map<std::string, std::optional<std::string>> SettingReaderCsv::GetSettings() {
    std::map<std::string, std::optional<std::string>> out;

    std::ifstream infile(m_path);
    if (!infile.is_open()) {
        log.warning() << "Failed to open " << m_path;
        return out;
    }

    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (!(iss >> key >> value)) {
            log.warning() << "Failed to parse line, ignoring: \"" << line << "\"";
            continue;
        }
        // Allow setting clearing with syntax 'DELETE <key>'
        if (key == "DELETE") {
            out[value] = std::nullopt;
        } else {
            out[key] = value;
        }
    }
    return out;
}

void SettingReaderCsv::write(std::map<std::string, std::optional<std::string>> &settings) {
    std::ofstream file(m_path);
    if (!file.is_open()) {
        log.warning() << "Failed to open " << m_path << ", cannot write settings!";
        return;
    }
    for ( auto &[key, value] : settings ) {
        if (value.has_value()) {
            file << key << " " << value.value() << std::endl;
        } else {
            file << "DELETE " << key << std::endl;
        }
    }
}
