#include "settingLayer.h"
#include <sstream>

bool SettingLayerHandler::handle_args(std::vector<std::pair<std::string, bool>> &args, Log &log) {
    bool ret = false;
    for (int i = 0; i < args.size(); i++) {
        if (args[i].first == "-h" || args[i].first == "--help") {
            args[i].second = true;

            log.none() << "Options - Layers";
            log.none() << " -l, --layers <l1>,<l2>...    (required) Space-separated list of layers to use, ordered lowest to highest priority";
            log.none();
            return true;
        }
        if (args[i].first == "-l" || args[i].first == "--layers") {
            if (i+1 < args.size()) {
                initialize(args[i+1].first);

                // Consume flag and arg, jump ahead
                args[i].second = true;
                args[i+1].second = true;
                i++;
                ret = true;
            }
        }
    }
    return ret;
}

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
