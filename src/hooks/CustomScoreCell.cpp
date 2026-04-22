#include <Geode/modify/GJScoreCell.hpp>
#include <defs/geode.hpp>
#include <ui/moreleaderboards/MoreLeaderboards.hpp>
#include <ui/moreleaderboards/LevelHelper.hpp>

class $modify(CustomScoreCell, GJScoreCell) {
    void loadFromScore(GJUserScore* score) {
        GJScoreCell::loadFromScore(score);

        CCLayer* layer = static_cast<CCLayer*>(this->getChildren()->objectAtIndex(1));
        if (!layer) return;

        std::string data_flag = MoreLeaderboards::data_response_moreLB;
        if (data_flag == "") return;

        RowLayout* layout = RowLayout::create();
        layout->setGap(3);
        layout->setAxisAlignment(AxisAlignment::Start);
        layout->setCrossAxisOverflow(true);

        CCMenu* menu = CCMenu::create();
        menu->setContentSize(CCSize(283, 21.580f));
        menu->setPosition(206, 42);

        auto starsLabel = static_cast<CCLabelBMFont*>(layer->getChildByIDRecursive("stars-label"));
        starsLabel->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0, 0.5F)->setScalePriority(1));
        auto starsIcon = static_cast<CCSprite*>(layer->getChildByIDRecursive("stars-icon"));
        starsIcon->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0, 0.5F)->setScalePriority(1));
        auto moonsLabel = static_cast<CCLabelBMFont*>(layer->getChildByIDRecursive("moons-label"));
        moonsLabel->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0, 0.5F)->setScalePriority(1));
        auto moonsIcon = static_cast<CCSprite*>(layer->getChildByIDRecursive("moons-icon"));
        moonsIcon->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0, 0.5F)->setScalePriority(1));

        auto* playerNameItem = static_cast<CCMenuItemSpriteExtra*>(layer->getChildByIDRecursive("player-name"));
        std::string playerName = static_cast<CCLabelBMFont*>(playerNameItem->getChildren()->objectAtIndex(0))->getString();
        CCMenu* menu_playername = CCMenu::create();
        menu_playername->setContentSize(playerNameItem->getContentSize());
        menu_playername->setLayout(RowLayout::create()->setGap(3)->setAxisAlignment(AxisAlignment::Start));

        auto playerNameBM = CCLabelBMFont::create(playerName.c_str(), "goldFont.fnt");
        auto playerNameBtn = CCMenuItemSpriteExtra::create(
            playerNameBM, this, (SEL_MenuHandler)&CustomScoreCell::onViewProfile
        );
        playerNameBtn->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0.5F, 0.8F)->setScalePriority(1));

        std::vector<std::string> dataString = MoreLeaderboards::getWords(data_flag, "#");
        std::vector<std::string> users = MoreLeaderboards::getWords(dataString[0], "|");
        std::string count_diff = "0";

        while (users.size() > 0) {
            std::vector<std::string> data_info = MoreLeaderboards::getWords(users[0], ":");
            while (data_info.size() > 0) {
                std::string id = data_info[0];
                std::string info = data_info[1];
                if (id == "1") {
                    if (info != playerName) break;
                }
                if (id == "9989") {
                    count_diff = info;
                }
                if (id == "9990") {
                    if (info == "XX") break;
                    auto flagSpr = CCSprite::createWithSpriteFrameName((""_spr + info + ".png").c_str());
                    flagSpr->setPosition({20, 15});
                    flagSpr->setScaleY(20 / flagSpr->getContentHeight() * 0.50);
                    flagSpr->setScaleX(std::min(1.0f, 30 / flagSpr->getContentWidth()));
                    flagSpr->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(true)->setScalePriority(1));
                    menu_playername->addChild(flagSpr);
                }
                data_info.erase(data_info.begin());
                data_info.erase(data_info.begin());
            }
            users.erase(users.begin());
        }

        menu_playername->addChild(playerNameBtn);
        menu_playername->updateLayout();

        if (auto n = layer->getChildByID("stars-label"))  n->removeFromParentAndCleanup(true);
        if (auto n = layer->getChildByID("stars-icon"))   n->removeFromParentAndCleanup(true);
        if (auto n = layer->getChildByID("moons-label"))  n->removeFromParentAndCleanup(true);
        if (auto n = layer->getChildByID("moons-icon"))   n->removeFromParentAndCleanup(true);
        if (auto n = layer->getChildByID("main-menu"))    n->removeFromParentAndCleanup(true);

        menu->addChild(menu_playername);
        menu->addChild(starsLabel);
        menu->addChild(starsIcon);
        menu->addChild(moonsLabel);
        menu->addChild(moonsIcon);
        menu->setLayout(layout);
        menu->updateLayout();

        // find the main layer with stats
        CCLayer* mainLayer = nullptr;
        for (auto* child : this->getChildrenExt()) {
            auto* search = typeinfo_cast<CCLayer*>(child);
            if (search && search->getChildrenCount() > 2) {
                mainLayer = search;
                break;
            }
        }
        if (!mainLayer) { layer->addChild(menu); return; }
        auto statsMenu = mainLayer->getChildByIDRecursive("stats-menu");

        if (!Loader::get()->isModLoaded("itzkiba.better_progression") && MoreLeaderboards::g_tab == StatsListType::BetterProgression) {
            int currentEXP = LevelHelper::calculateTotalEXP(score);
            int currentLevel = LevelHelper::getLevelFromEXP(currentEXP);
            auto badgeSpr = SpriteHelper::createFullSprite(currentLevel);
            badgeSpr->setPosition({75, 17.5});
            badgeSpr->setScale(0.62);
            mainLayer->addChild(badgeSpr);
            statsMenu->setScale(0.83);
            statsMenu->setPositionX(statsMenu->getPositionX() + 13.f);
        }

        auto makeClassicSpr = [](const char* base) {
            auto s = CCSprite::createWithSpriteFrameName(base);
            auto c = CCSprite::createWithSpriteFrameName("star_small01_001.png");
            c->setPositionX(30); c->setScale(1.2f);
            s->addChild(c);
            return s;
        };
        auto makePlatformerSpr = [](const char* base) {
            auto s = CCSprite::createWithSpriteFrameName(base);
            auto m = CCSprite::createWithSpriteFrameName("moon_small01_001.png");
            m->setPositionX(30); m->setScale(1.2f);
            s->addChild(m);
            return s;
        };
        auto pointercrate = CCSprite::createWithSpriteFrameName("diffIcon_06_btn_001.png");
        auto pc_text = CCLabelBMFont::create("PC", "goldFont.fnt");
        pc_text->setPosition(28, 0); pc_text->setScale(0.5f);
        pointercrate->addChild(pc_text);

        if (statsMenu) {
            const struct { const char* id; int z; } zmap[] = {
                {"diamonds-label", 0}, {"diamonds-icon", 1},
                {"coins-label", 2},    {"coins-icon", 3},
                {"user-coins-label", 4}, {"user-coins-icon", 5},
                {"demons-label", 6},   {"demons-icon", 7},
                {"creator-points-label", 8}, {"creator-points-icon", 9},
            };
            for (auto& e : zmap) {
                if (auto n = statsMenu->getChildByIDRecursive(e.id)) n->setZOrder(e.z);
            }
        }

        auto demon_icon = mainLayer->getChildByIDRecursive("demons-icon");
        auto demon_label = mainLayer->getChildByIDRecursive("demons-label");
        if (!demon_icon || !demon_label) { layer->addChild(menu); return; }

        auto demon_icon_pos    = demon_icon->getPosition();
        auto demon_icon_scale  = demon_icon->getScale();
        auto demon_icon_z      = demon_icon->getZOrder();
        auto demon_icon_tag    = demon_icon->getTag();
        auto demon_icon_layout = demon_icon->getLayoutOptions();
        auto demon_label_pos    = demon_label->getPosition();
        auto demon_label_scale  = demon_label->getScale();
        auto demon_label_z      = demon_label->getZOrder();
        auto demon_label_tag    = demon_label->getTag();
        auto demon_label_layout = demon_label->getLayoutOptions();

        CCSprite* new_icon = nullptr;
        switch (MoreLeaderboards::g_tab) {
            case StatsListType::platformerDemonsEasy:    new_icon = makePlatformerSpr("diffIcon_07_btn_001.png"); break;
            case StatsListType::platformerDemonsMedium:  new_icon = makePlatformerSpr("diffIcon_08_btn_001.png"); break;
            case StatsListType::platformerDemonsHard:    new_icon = makePlatformerSpr("diffIcon_06_btn_001.png"); break;
            case StatsListType::platformerDemonsInsane:  new_icon = makePlatformerSpr("diffIcon_09_btn_001.png"); break;
            case StatsListType::platformerDemonsExtreme: new_icon = makePlatformerSpr("diffIcon_10_btn_001.png"); break;
            case StatsListType::classicDemonsEasy:       new_icon = makeClassicSpr("diffIcon_07_btn_001.png"); break;
            case StatsListType::classicDemonsMedium:     new_icon = makeClassicSpr("diffIcon_08_btn_001.png"); break;
            case StatsListType::classicDemonsHard:       new_icon = makeClassicSpr("diffIcon_06_btn_001.png"); break;
            case StatsListType::classicDemonsInsane:     new_icon = makeClassicSpr("diffIcon_09_btn_001.png"); break;
            case StatsListType::classicDemonsExtreme:    new_icon = makeClassicSpr("diffIcon_10_btn_001.png"); break;
            case StatsListType::Pointercrate:            new_icon = pointercrate; break;
            default: break;
        }

        if (new_icon && statsMenu) {
            demon_icon->removeFromParent();
            new_icon->setPosition(demon_icon_pos);
            new_icon->setScale(demon_icon_scale);
            new_icon->setLayoutOptions(demon_icon_layout);
            new_icon->setTag(demon_icon_tag);
            new_icon->setZOrder(demon_icon_z);
            statsMenu->addChild(new_icon);

            demon_label->removeFromParent();
            auto new_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
            new_label->setPosition(demon_label_pos);
            new_label->setScale(demon_label_scale);
            new_label->setZOrder(demon_label_z);
            new_label->setTag(demon_label_tag);
            new_label->setLayoutOptions(demon_label_layout);
            statsMenu->addChild(new_label);
            statsMenu->updateLayout();
        }

        layer->addChild(menu);
        layer->updateLayout();
    }
};
