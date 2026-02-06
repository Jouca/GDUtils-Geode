#pragma once
#include <defs/geode.hpp>
enum class PositionEnum : int {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};
template <>
struct matjson::Serialize<PositionEnum> {
    static matjson::Value toJson(PositionEnum const& value) {
        switch (value) {
            case PositionEnum::TopLeft:
                return 1;
            case PositionEnum::TopRight:
                return 2;
            case PositionEnum::BottomLeft:
                return 3;
            default:
            case PositionEnum::BottomRight:
                return 4;
        }
    }
    static Result<PositionEnum> fromJson(matjson::Value const& value) {
        switch (value.asInt().unwrapOrDefault()) {
            case 1: return Ok(PositionEnum::TopLeft);
            case 2: return Ok(PositionEnum::TopRight);
            case 3: return Ok(PositionEnum::BottomLeft);
            case 4: return Ok(PositionEnum::BottomRight);
            default: return Err("Invalid PositionEnum value '{}'", value.asInt().unwrapOrDefault());
        }
    }
    static bool is_json(matjson::Value const& json) {
        return json.isNumber() || json.isString();
    }
};

class PositionValue : public SettingBaseValueV3<PositionEnum> {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<PositionValue>();
        auto root = checkJson(json, "PositionValue");
        res->parseBaseProperties(key, modID, root);
        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }
    SettingNodeV3* createNode(float width) override;
};


template <>
struct geode::SettingTypeForValueType<PositionEnum> {
    using SettingType = PositionValue;
};

class PositionNode : public SettingValueNodeV3<PositionValue> {
protected:
    CCMenuItemToggler* m_tlBtn;
    CCMenuItemToggler* m_trBtn;
    CCMenuItemToggler* m_blBtn;
    CCMenuItemToggler* m_brBtn;
    bool init(std::shared_ptr<PositionValue> setting, float width);
    void updateState(CCNode* invoker) override {
        SettingValueNodeV3::updateState(invoker);
    }
    void onCornerClick(PositionEnum value) {
        m_tlBtn->toggle(true);
        m_trBtn->toggle(true);
        m_blBtn->toggle(true);
        m_brBtn->toggle(true);
        this->setValue(value, nullptr);
    };
    void onResetToDefault() override {
        m_tlBtn->toggle(true);
        m_trBtn->toggle(true);
        m_blBtn->toggle(true);
        m_brBtn->toggle(false);
        this->setValue(PositionEnum::BottomRight, nullptr);
    }
public:
    static PositionNode* create(std::shared_ptr<PositionValue> setting, float width) {
        auto ret = new PositionNode();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
