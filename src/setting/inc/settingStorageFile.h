#pragma once

#include "settingSource.h"

class SettingStorageFile : public ISettingStorage {
public:
    SettingStorageFile(Log &logger) : ISettingStorage("File"), log(logger.getChild("file")) {};
    bool handle_args(std::vector<std::pair<std::string, bool>> &args, Log &log) override;
    std::map<std::string, std::pair<SettingLayer*, std::optional<std::string>>> GetSettings() override;
    const std::string LayerAlias(SettingLayer*) override;
    void Flush(SettingLayer *layer) override;

private:
    std::string m_basePath;
    Log log;
};
