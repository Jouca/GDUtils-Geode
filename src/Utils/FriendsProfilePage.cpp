#include "../includes.h"
#include "../BrownAlertDelegate.hpp"
#include <Geode/modify/FriendsProfilePage.hpp>
// Utils

// Function to convert a const char* to lowercase
const char* toLowerCase(const char* str) {
    char* result = new char[strlen(str) + 1];
    size_t i = 0;

    while (str[i]) {
        result[i] = std::tolower(str[i]);
        i++;
    }

    result[i] = '\0';
    return result;
}

class SearchUserLayer : public BrownAlertDelegate {
    protected:
        virtual void setup() {
            // Set up the alert layer
            input_username->setMaxCharCount(20);
            input_username->setPlaceholder("");
            input_username->setPositionY(10);

            this->m_buttonMenu->addChild(input_username);

            auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

            // Add search button
            auto validate_spr = ButtonSprite::create("Search", 60, true, "bigFont.fnt", "GJ_button_01.png", 30, .5F);
            auto validate_btn = CCMenuItemSpriteExtra::create(
                validate_spr,
                this,
                menu_selector(SearchUserLayer::onValidate)
            );
            validate_btn->setPosition({
                0,
                -35
            });
            this->m_buttonMenu->addChild(validate_btn, 1);
            
            // Add the menu to the layer
            this->m_mainLayer->addChild(this->m_buttonMenu);

            // Enable touch
            setTouchEnabled(true);
        }
        cocos2d::CCSize m_sScrLayerSize;
        void onClose(cocos2d::CCObject* pSender) {
            BrownAlertDelegate::onClose(pSender);
        }
        void onValidate(cocos2d::CCObject*);
        float m_fWidth = s_defWidth;
        float m_fHeight = s_defHeight;
    public:
        TextInput* input_username = TextInput::create(200.0F, "Username", "bigFont.fnt");
        static constexpr const float s_defWidth = 260.0f;
        static constexpr const float s_defHeight = 120.0f;
        static SearchUserLayer* create() {
            auto pRet = new SearchUserLayer();
            if (pRet && pRet->init(SearchUserLayer::s_defWidth, SearchUserLayer::s_defHeight, "GJ_square01.png")) {
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
};

class $modify(FriendPage, FriendsProfilePage) {
    bool init(UserListType type) {
        // If the base init fails, stop here
        if (!FriendsProfilePage::init(type)) return false;
        // If the setting is disabled, stop here
        if (!Mod::get()->template getSettingValue<bool>("friendSearch")) return true;

        auto menu = this->m_buttonMenu;

        auto searchSpr = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
        searchSpr->setScale(1.225F);
        auto searchBtn = CCMenuItemSpriteExtra::create(
            searchSpr,
            this,
            menu_selector(FriendPage::onSearch)
        );
        searchBtn->setPosition(320, 0);
        menu->addChild(searchBtn);
        return true;
    }
    
    void onSearch(CCObject*) {
        SearchUserLayer::create()->show();
    }

    static TableView* getCustomList(CCArray* sceneChildren) {
        CCLayer* test1 = typeinfo_cast<CCLayer*>(misc::findNode("FriendsProfilePage"));
        if (test1 == nullptr) {
            // safeguard from crashing
            FLAlertLayer::create(nullptr,
                "Error",
                "The mod could not find the <cy>FriendsProfilePage</c> layer. Please either <cg>try again later</c>, removing mods that may be interfering with the scene, or report this to the developers.",
                "OK",
                nullptr,
                350.0F
            )->show();
            return nullptr;
        }
        test1 = typeinfo_cast<CCLayer*>(test1->getChildren()->objectAtIndex(0));
        CCLayer* test2 = nullptr;
        for (int i = 0; i < test1->getChildrenCount(); i++) {
            if (misc::getNodeName(test1->getChildren()->objectAtIndex(i)) == "GJCommentListLayer") {
                test2 = static_cast<CCLayer*>(test1->getChildren()->objectAtIndex(i));
                break;
            }
        }

        // If we couldn't find the comment list layer, return
        if (test2 == nullptr) {
            // safeguard from crashing
            FLAlertLayer::create(nullptr,
                "Error",
                "The mod could not find the <cy>GJCommentListLayer</c> layer. Please either <cg>try again later</c>, removing mods that may be interfering with the scene, or report this to the developers.",
                "OK",
                nullptr,
                350.0F
            )->show();
            return nullptr;
        }

        auto test3 = static_cast<CCLayer*>(test2->getChildren()->objectAtIndex(0));
        
        // If you don't have friend, show alert (sorry for you)
        if (test3->getChildrenCount() <= 0) {
            // another safeguard
            FLAlertLayer::create(nullptr,
                "Error",
                "You have <cy>no friends</c>!",
                "OK",
                nullptr,
                200.0F
            )->show();
            return nullptr;
        }

        return static_cast<TableView*>(test3->getChildren()->objectAtIndex(0));
    }

    static void searchUser(const char* username) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto sceneChildren = scene->getChildren();
        auto customList = getCustomList(sceneChildren);

        // If the list is null, return
        if (customList == nullptr) return;

        CCContentLayer* contentLayer = static_cast<CCContentLayer*>(
            customList->getChildren()->objectAtIndex(0)
        );

        int counter_page = 0;
        bool found = false;

        // Iterate through all cells in the list
        for (int i = 0; i < contentLayer->getChildrenCount(); i++) {
            GJUserCell* cell = static_cast<GJUserCell*>(
                contentLayer->getChildren()->objectAtIndex(i)
            );

            const char* str1 = cell->m_userScore->m_userName.c_str();

            // If username is found in the cell's username (case insensitive)
            if (strstr(toLowerCase(str1), toLowerCase(username)) != nullptr) {
                customList->scrollLayer(-9999999);
                customList->scrollLayer(counter_page);

                found = true;

                // Pulse only one time cell color animation
                auto originalColor = cell->getChildByType<cocos2d::CCLayerColor>(0)->getColor();
                auto tintTo1 = CCTintTo::create(0.2f, 0, 255, 0); // Change to green
                auto tintTo2 = CCTintTo::create(0.5f, originalColor.r, originalColor.g, originalColor.b); // Change back to original color
                auto sequence = CCSequence::create(tintTo1, tintTo2, nullptr);
                cell->getChildByType<cocos2d::CCLayerColor>(0)->runAction(sequence);

                break;
            }
            counter_page += 45;
        }

        // If user not found, show alert
        if (!found) {
            std::string str = username;
            FLAlertLayer::create(nullptr,
                "Error",
                "User not found : " + str,
                "OK",
                nullptr,
                200.0F
            )->show();
        }
    }

    void setupUsersBrowser(cocos2d::CCArray * users, UserListType type)
    {
        FriendsProfilePage::setupUsersBrowser(users, type);
     
        if (m_buttonMenu->getChildByID("friendsScrollBar"))
        {
            m_buttonMenu->removeChildByID("friendsScrollBar");
        }
    }

    virtual void getUserListFinished(cocos2d::CCArray* a1, UserListType a2) {
        FriendsProfilePage::getUserListFinished(a1,a2);

        auto menu = this->m_buttonMenu;
        
        // Add custom scrollbar
        auto scrollBar = Scrollbar::create(this->m_listLayer->m_list->m_tableView);
        scrollBar->setPosition(390, -140);
        scrollBar->setID("friendsScrollBar");
        menu->addChild(scrollBar);
    }

    virtual void onUpdate(UserListType a1) {
        FriendsProfilePage::forceReloadList(a1);
    
        m_buttonMenu->removeChildByID("friendsScrollBar");
    }
};

void SearchUserLayer::onValidate(CCObject* pSender) {
    FriendPage::searchUser(input_username->getString().c_str());
    BrownAlertDelegate::onClose(pSender);
}