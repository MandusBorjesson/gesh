
#include "settingReaderFile.h"
#include <fstream>
#include <map>
#include <sstream>


SettingReaderCsv::SettingReaderCsv(const std::string &path, Log &logger) : ISettingReader(path), m_path(path), log(logger.getChild("file")) {
    log.info() << "Created for path " << path;
};

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
