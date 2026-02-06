#pragma once
#include <defs/geode.hpp>

class CreditsValue : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<CreditsValue>();
        auto root = checkJson(json, "SettingCreditsValue");
        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        res->parseEnableIf(root);
        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }
    bool isDefaultValue() const override {
        return true;
    }
    void reset() override {}
    SettingNodeV3* createNode(float width) override;
};
class CreditsNode : public SettingNodeV3 {
protected:
    bool init(std::shared_ptr<CreditsValue> setting, float width);
    void onCommit() override {}
    void onResetToDefault() override {}
public:
    bool hasUncommittedChanges() const override {
        return false;
    }
    bool hasNonDefaultValue() const override {
        return false;
    }
    static CreditsNode* create(std::shared_ptr<CreditsValue> setting, float width) {
        auto ret = new CreditsNode();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
