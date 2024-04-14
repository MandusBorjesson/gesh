#include "settingLayer.h"
#include <sstream>

void SettingLayerHandler::initialize(std::string layers) {
    std::stringstream ss(layers);
    std::vector<std::string> result;

    int i = 0;
    while( ss.good() )
    {
        std::string substr;
        std::getline( ss, substr, ',' );
        m_layers.push_back(SettingLayer(substr, i));
        i++;
    }
}

SettingLayer* SettingLayerHandler::findLayer(std::string name) {
    for (auto & layer : m_layers) {
        if (name == layer.name()) {
            return &layer;
        }
    }
    return nullptr;
}

std::vector<SettingLayer*> SettingLayerHandler::layers() {
    std::vector<SettingLayer*> ret;
    for (auto & layer : m_layers) {
        ret.push_back(&layer);
    }
    return ret;
}
