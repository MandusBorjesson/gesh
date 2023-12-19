#pragma once

#include "ISettingSource.h"
#include <vector>

class SettingSourceDefault : public ISettingSource {
public:
    SettingSourceDefault() : ISettingSource("DEFAULT") {};
    std::vector<ISettingSource::Setting> GetSettings() override;
};
