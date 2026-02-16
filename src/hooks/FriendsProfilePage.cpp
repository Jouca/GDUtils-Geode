#include <defs/geode.hpp>
#include <Geode/modify/FriendsProfilePage.hpp>
#include <ui/SearchUserLayer.hpp>

class $modify(FriendPage, FriendsProfilePage) {
    bool init(UserListType type) {
        if (!FriendsProfilePage::init(type)) return false;
        Build<CCSprite>::createSpriteName("gj_findBtn_001.png").intoMenuItem([this]() {
            if (!m_users) {
    youhavenofriends:
                FLAlertLayer::create(nullptr,
                    "Error",
                    "You have <cy>no friends</c>!",
                    "OK",
                    nullptr,
                    200.0F
                )->show();
                return;
            }
            if (!m_listLayer->m_list)
                return;
            auto customList = m_listLayer->m_list->m_tableView;
            if (customList == nullptr) return;
            if (customList->m_contentLayer && customList->m_contentLayer->getChildrenCount() <= 0) {
                goto youhavenofriends;
            }
            SearchUserLayer::create(this)->show();
        }).pos(15, -135).parent(m_buttonMenu);
        return true;
    }
    void setupUsersBrowser(cocos2d::CCArray * users, UserListType type) {
        FriendsProfilePage::setupUsersBrowser(users, type);
        m_buttonMenu->removeChildByID("friendsScrollBar"_spr);
    }
    virtual void getUserListFinished(cocos2d::CCArray* a1, UserListType a2) {
        FriendsProfilePage::getUserListFinished(a1,a2);
        // Add custom scrollbar
        auto scrollBar = Scrollbar::create(m_listLayer->m_list->m_tableView);
        scrollBar->setPosition(390, -140);
        scrollBar->setID("friendsScrollBar"_spr);
        m_buttonMenu->addChild(scrollBar);
    }
    virtual void onUpdate(UserListType a1) {
        FriendsProfilePage::forceReloadList(a1);
        m_buttonMenu->removeChildByID("friendsScrollBar"_spr);
    }
};

void SearchUserLayer::onValidate() {
    if (auto obj = m_friendPage.lock()) {
        std::string username = input_username->getString();
        // If we couldn't find the comment list layer, return
        if (obj->m_listLayer == nullptr) {
            // safeguard from crashing
            FLAlertLayer::create(nullptr,
                "Error",
                "The mod could not find the <cy>GJCommentListLayer</c> layer. Please either <cg>try again later</c>, removing mods that may be interfering with the scene, or report this to the developers.",
                "OK",
                nullptr,
                350.0F
            )->show();
            return;
        }
        if (!obj->m_users) {
youhavenofriends:
            FLAlertLayer::create(nullptr,
                "Error",
                "You have <cy>no friends</c>!",
                "OK",
                nullptr,
                200.0F
            )->show();
            return;
        }
        if (!obj->m_listLayer->m_list)
            return;
        auto customList = obj->m_listLayer->m_list->m_tableView;

        // If the list is null, return
        if (customList == nullptr) return;

        if (customList->m_contentLayer && customList->m_contentLayer->getChildrenCount() <= 0) {
            goto youhavenofriends;
        }

        auto contentLayer = customList->m_contentLayer;

        int counter_page = 0;
        bool found = false;

        auto cells = CCArrayExt<GJUserCell*>(customList->m_contentLayer->getChildren());

        // Iterate through all cells in the list
        for (auto cell : cells) {
            std::string inputUsername = geode::utils::string::toLower(cell->m_userScore->m_userName);

            // If username is found in the cell's username (case insensitive)
            if (inputUsername.find(geode::utils::string::toLower(username)) != std::string::npos) {
                customList->scrollLayer(-9999999);
                customList->scrollLayer(counter_page);
                found = true;
                // Pulse only one time cell color animation
                if (auto layer = cell->getChildByType<cocos2d::CCLayerColor>(0)) {
                    auto originalColor = layer->getColor();
                    auto tintTo1 = CCTintTo::create(0.2f, 0, 255, 0); // Change to green
                    auto tintTo2 = CCTintTo::create(0.5f, originalColor.r, originalColor.g, originalColor.b); // Change back to original color
                    auto sequence = CCSequence::create(tintTo1, tintTo2, nullptr);
                    cell->getChildByType<cocos2d::CCLayerColor>(0)->runAction(sequence);
                }
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
    Popup::onClose(nullptr);
}

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName().starts_with("FriendsProfilePage")) {
            if (value) {
                (void)hook->enable();
            } else {
                (void)hook->disable();
            }
        }
    }
}

$execute {
    listenForSettingChanges<bool>("friendSearch", [](bool value) {
        log::info("Friend Search changed to {}", value);
        toggleHook(value);
    });
}

$on_mod(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("friendSearch")) {
        toggleHook(false);
    }
}
