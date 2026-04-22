#pragma once
#include <defs/geode.hpp>

#define STAR_EXP 5
#define MOON_EXP 5
#define DIAMOND_EXP 2
#define GOLD_COIN_EXP 100
#define USER_COIN_EXP 20
#define DEMON_EXP 75
#define CP_EXP 5000

class LevelHelper {
public:
    inline static int calculateTotalEXP(GJUserScore* score) {
        int totalEXP = 0;
        totalEXP += (STAR_EXP * score->m_stars);
        totalEXP += (MOON_EXP * score->m_moons);
        totalEXP += (DIAMOND_EXP * score->m_diamonds);
        totalEXP += (GOLD_COIN_EXP * score->m_secretCoins);
        totalEXP += (USER_COIN_EXP * score->m_userCoins);
        totalEXP += (DEMON_EXP * score->m_demons);
        totalEXP += (CP_EXP * score->m_creatorPoints);
        return totalEXP;
    }

    inline static int getEXPRequiredForLevel(int level) {
        return (50 * (level * level) + (50 * level));
    }

    inline static int getLevelFromEXP(int exp) {
        return (sqrt(2 * exp + 25) - 5) / 10.0;
    }
};

class SpriteHelper {
public:
    inline static CCSprite* createFullSprite(int currentLevel) {
        auto badgeSprite = SpriteHelper::getBadgeFromLevel(currentLevel);
        badgeSprite->setID("itzkiba.better_progression/tier-badge");
        badgeSprite->setZOrder(10);

        auto badgeLabel = CCLabelBMFont::create(std::to_string(currentLevel).c_str(), "bigFont.fnt");
        badgeLabel->setZOrder(11);
        badgeLabel->setScale((currentLevel >= 100 ? 0.3 : 0.4));
        badgeLabel->setPosition({badgeSprite->getContentSize().width / 2, badgeSprite->getContentSize().height / 2 + 1.f});
        badgeSprite->addChild(badgeLabel);

        auto badgeShadow = CCSprite::createWithSpriteFrameName("itzkiba.better_progression/shadow.png");
        badgeShadow->setZOrder(-5);
        badgeShadow->setScale(0.9);
        badgeShadow->setPosition({badgeSprite->getContentSize().width / 2, badgeSprite->getContentSize().height / 2});
        badgeShadow->setOpacity(100);
        badgeSprite->addChild(badgeShadow);

        auto badgeGlow = SpriteHelper::getGlowFromLevel(currentLevel);
        if (badgeGlow != nullptr && currentLevel <= 350) {
            badgeGlow->setZOrder(-3);
            badgeGlow->setPosition({badgeSprite->getContentSize().width / 2, badgeSprite->getContentSize().height / 2});
            badgeGlow->setScaleX(0.975);
            badgeGlow->setScaleY(0.95);
            badgeSprite->addChild(badgeGlow);

            ccBlendFunc blending = {GL_ONE, GL_ONE};
            badgeGlow->setBlendFunc(blending);

            if (currentLevel >= 200 && currentLevel <= 224) badgeGlow->setColor({255, 156, 110});
            if (currentLevel >= 225 && currentLevel <= 249) badgeGlow->setColor({90, 255, 148});
            if (currentLevel >= 250 && currentLevel <= 274) badgeGlow->setColor({65, 239, 255});
            if (currentLevel >= 275 && currentLevel <= 299) badgeGlow->setColor({245, 108, 255});

            auto fadein = CCEaseSineInOut::create(CCFadeTo::create(0.9f, 50));
            auto fadeout = CCEaseSineInOut::create(CCFadeTo::create(0.9f, 255));
            auto sequence = CCSequence::create(fadein, fadeout, nullptr);
            badgeGlow->runAction(CCRepeatForever::create(sequence));
        }
        return badgeSprite;
    }

    inline static CCSprite* getBadgeFromLevel(int level) {
        const char* frame = "itzkiba.better_progression/tier0_0.png";
        if (level >= 5   && level <= 9)   frame = "itzkiba.better_progression/tier0_1.png";
        if (level >= 10  && level <= 14)  frame = "itzkiba.better_progression/tier0_2.png";
        if (level >= 15  && level <= 19)  frame = "itzkiba.better_progression/tier0_3.png";
        if (level >= 20  && level <= 24)  frame = "itzkiba.better_progression/tier0_4.png";
        if (level >= 25  && level <= 29)  frame = "itzkiba.better_progression/tier1_0.png";
        if (level >= 30  && level <= 34)  frame = "itzkiba.better_progression/tier1_1.png";
        if (level >= 35  && level <= 39)  frame = "itzkiba.better_progression/tier1_2.png";
        if (level >= 40  && level <= 44)  frame = "itzkiba.better_progression/tier1_3.png";
        if (level >= 45  && level <= 49)  frame = "itzkiba.better_progression/tier1_4.png";
        if (level >= 50  && level <= 54)  frame = "itzkiba.better_progression/tier2_0.png";
        if (level >= 55  && level <= 59)  frame = "itzkiba.better_progression/tier2_1.png";
        if (level >= 60  && level <= 64)  frame = "itzkiba.better_progression/tier2_2.png";
        if (level >= 65  && level <= 69)  frame = "itzkiba.better_progression/tier2_3.png";
        if (level >= 70  && level <= 74)  frame = "itzkiba.better_progression/tier2_4.png";
        if (level >= 75  && level <= 79)  frame = "itzkiba.better_progression/tier3_0.png";
        if (level >= 80  && level <= 84)  frame = "itzkiba.better_progression/tier3_1.png";
        if (level >= 85  && level <= 89)  frame = "itzkiba.better_progression/tier3_2.png";
        if (level >= 90  && level <= 94)  frame = "itzkiba.better_progression/tier3_3.png";
        if (level >= 95  && level <= 99)  frame = "itzkiba.better_progression/tier3_4.png";
        if (level >= 100 && level <= 104) frame = "itzkiba.better_progression/tier4_0.png";
        if (level >= 105 && level <= 109) frame = "itzkiba.better_progression/tier4_1.png";
        if (level >= 110 && level <= 114) frame = "itzkiba.better_progression/tier4_2.png";
        if (level >= 115 && level <= 119) frame = "itzkiba.better_progression/tier4_3.png";
        if (level >= 120 && level <= 124) frame = "itzkiba.better_progression/tier4_4.png";
        if (level >= 125 && level <= 129) frame = "itzkiba.better_progression/tier5_0.png";
        if (level >= 130 && level <= 134) frame = "itzkiba.better_progression/tier5_1.png";
        if (level >= 135 && level <= 139) frame = "itzkiba.better_progression/tier5_2.png";
        if (level >= 140 && level <= 144) frame = "itzkiba.better_progression/tier5_3.png";
        if (level >= 145 && level <= 149) frame = "itzkiba.better_progression/tier5_4.png";
        if (level >= 150 && level <= 154) frame = "itzkiba.better_progression/tier6_0.png";
        if (level >= 155 && level <= 159) frame = "itzkiba.better_progression/tier6_1.png";
        if (level >= 160 && level <= 164) frame = "itzkiba.better_progression/tier6_2.png";
        if (level >= 165 && level <= 169) frame = "itzkiba.better_progression/tier6_3.png";
        if (level >= 170 && level <= 174) frame = "itzkiba.better_progression/tier6_4.png";
        if (level >= 175 && level <= 179) frame = "itzkiba.better_progression/tier7_0.png";
        if (level >= 180 && level <= 184) frame = "itzkiba.better_progression/tier7_1.png";
        if (level >= 185 && level <= 189) frame = "itzkiba.better_progression/tier7_2.png";
        if (level >= 190 && level <= 194) frame = "itzkiba.better_progression/tier7_3.png";
        if (level >= 195 && level <= 199) frame = "itzkiba.better_progression/tier7_4.png";
        if (level >= 200 && level <= 204) frame = "itzkiba.better_progression/tier8_0.png";
        if (level >= 205 && level <= 209) frame = "itzkiba.better_progression/tier8_1.png";
        if (level >= 210 && level <= 214) frame = "itzkiba.better_progression/tier8_2.png";
        if (level >= 215 && level <= 219) frame = "itzkiba.better_progression/tier8_3.png";
        if (level >= 220 && level <= 224) frame = "itzkiba.better_progression/tier8_4.png";
        if (level >= 225 && level <= 229) frame = "itzkiba.better_progression/tier9_0.png";
        if (level >= 230 && level <= 234) frame = "itzkiba.better_progression/tier9_1.png";
        if (level >= 235 && level <= 239) frame = "itzkiba.better_progression/tier9_2.png";
        if (level >= 240 && level <= 244) frame = "itzkiba.better_progression/tier9_3.png";
        if (level >= 245 && level <= 249) frame = "itzkiba.better_progression/tier9_4.png";
        if (level >= 250 && level <= 254) frame = "itzkiba.better_progression/tier10_0.png";
        if (level >= 255 && level <= 259) frame = "itzkiba.better_progression/tier10_1.png";
        if (level >= 260 && level <= 264) frame = "itzkiba.better_progression/tier10_2.png";
        if (level >= 265 && level <= 269) frame = "itzkiba.better_progression/tier10_3.png";
        if (level >= 270 && level <= 274) frame = "itzkiba.better_progression/tier10_4.png";
        if (level >= 275 && level <= 279) frame = "itzkiba.better_progression/tier11_0.png";
        if (level >= 280 && level <= 284) frame = "itzkiba.better_progression/tier11_1.png";
        if (level >= 285 && level <= 289) frame = "itzkiba.better_progression/tier11_2.png";
        if (level >= 290 && level <= 294) frame = "itzkiba.better_progression/tier11_3.png";
        if (level >= 295 && level <= 299) frame = "itzkiba.better_progression/tier11_4.png";
        if (level >= 300 && level <= 349) frame = "itzkiba.better_progression/tier12.png";
        return CCSprite::createWithSpriteFrameName(frame);
    }

    inline static CCSprite* getGlowFromLevel(int level) {
        if (level < 200) return nullptr;
        int mod = level % 25;
        int tier = mod / 5;
        const char* frames[] = {
            "itzkiba.better_progression/glow_0.png",
            "itzkiba.better_progression/glow_1.png",
            "itzkiba.better_progression/glow_2.png",
            "itzkiba.better_progression/glow_3.png",
            "itzkiba.better_progression/glow_4.png",
        };
        if (level >= 300 && level <= 349) return CCSprite::createWithSpriteFrameName("itzkiba.better_progression/glow_4.png");
        if (level >= 350) return nullptr;
        return CCSprite::createWithSpriteFrameName(frames[tier]);
    }
};
