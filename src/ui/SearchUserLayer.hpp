#pragma once
#include <defs/geode.hpp>

class SearchUserLayer : public Popup {
    protected:
        WeakRef<FriendsProfilePage> m_friendPage;

        bool init(FriendsProfilePage*);
        void onValidate();
    public:
        TextInput* input_username = TextInput::create(200.0F, "Username", "bigFont.fnt");
        static SearchUserLayer* create(FriendsProfilePage* layer) {
            auto ret = new SearchUserLayer();
            if (ret->init(layer)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }
};

