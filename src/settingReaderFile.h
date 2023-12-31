#pragma once

#include "settingSource.h"

class SettingReaderCsv : public ISettingReader {
public:
    SettingReaderCsv(const std::string &path) : ISettingReader(path), m_path(path) {};
    std::map<std::string, std::string> GetSettings() override;

private:
    std::string m_path;
};
