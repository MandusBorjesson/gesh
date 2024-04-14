#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>
#include "log.h"

class SettingLayer {
public:
    SettingLayer(std::string name, int priority) : m_name(name), m_prio(priority) {};
    std::string name() {return m_name;}
    int priority() {return m_prio;}
private:
    std::string m_name;
    int m_prio;
};

class SettingLayerHandler {
public:
    SettingLayerHandler() : m_default(SettingLayer("default", -100)) {};
    void initialize(std::string layers);
    SettingLayer* findLayer(std::string name);
    std::vector<SettingLayer*> layers();
    SettingLayer* defaultLayer() { return &m_default; };
private:
    SettingLayer m_default;
    std::vector<SettingLayer> m_layers;
};
