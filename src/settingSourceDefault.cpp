#include "settingSourceDefault.h"

std::vector<ISettingSource::Setting> SettingSourceDefault::GetSettings() {
    return std::vector<ISettingSource::Setting> {
        (struct Setting){"test", 1, this},
        (struct Setting){"General/test1", 1, this},
        (struct Setting){"General/test2", 123, this}
    };
}
