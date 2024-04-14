#pragma once

#include "settingSource.h"

class SettingReaderCsv : public ISettingReader {
public:
    SettingReaderCsv(const std::string &path, Log &logger);
    std::map<std::string, std::optional<std::string>> GetSettings() override;
    void write(std::map<std::string, std::optional<std::string>> &settings) override;

private:
    std::string m_path;
    Log log;
};
