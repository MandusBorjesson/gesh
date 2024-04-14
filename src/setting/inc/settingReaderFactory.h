#include "settingSource.h"

namespace Factory {
std::shared_ptr<ISettingReader> Reader(const std::string &path);
} // namespace Factory
