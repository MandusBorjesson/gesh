#pragma once

#include "settingSource.h"

class SettingStorageFile : public ISettingStorage {
public:
    SettingStorageFile(std::string &basePath, Log &logger) : ISettingStorage("File"), m_basePath(basePath), log(logger.getChild("file") ) {};
    std::map<std::string, std::pair<SettingLayer*, std::optional<std::string>>> GetSettings() override;
    const std::string LayerAlias(SettingLayer*) override;
    void Flush(SettingLayer *layer) override;

private:
    std::string m_basePath;
    Log log;
};
