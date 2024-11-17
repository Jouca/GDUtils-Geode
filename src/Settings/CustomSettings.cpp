#include "CustomSettings.hpp"
#include <Geode/loader/Dirs.hpp>
#include "../includes.h"

SettingNodeV3* SettingPosValue::createNode(float width) {
    return SettingPosNode::create(static_pointer_cast<SettingPosValue>(shared_from_this()), width);
}
SettingNodeV3* SettingCreditsValue::createNode(float width) {
    return SettingCreditsNode::create(static_pointer_cast<SettingCreditsValue>(shared_from_this()), width);
}
