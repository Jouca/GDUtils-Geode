#pragma once
#include "../includes.h"

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
            return (sqrt(2*exp + 25) - 5) / 10.0;
        }
};

class SpriteHelper {
    public:
        inline static CCSprite* createFullSprite(int currentLevel) {
            auto badgeSprite = SpriteHelper::getBadgeFromLevel(currentLevel);
            badgeSprite->setID("tier-badge"_spr);
            badgeSprite->setZOrder(10);

            auto badgeLabel = CCLabelBMFont::create(std::to_string(currentLevel).c_str(), "bigFont.fnt");
            badgeLabel->setZOrder(11);
            badgeLabel->setScale((currentLevel >= 100 ? 0.3 : 0.4));
            badgeLabel->setPosition({badgeSprite->getContentSize().width / 2, badgeSprite->getContentSize().height / 2 + 1.f});
            badgeSprite->addChild(badgeLabel);

            auto badgeShadow = CCSprite::createWithSpriteFrameName("shadow.png"_spr);
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

                if (currentLevel >= 200 && currentLevel <= 224)
                    badgeGlow->setColor({ 255, 156, 110 });
                if (currentLevel >= 225 && currentLevel <= 249)
                    badgeGlow->setColor({ 90, 255, 148 });
                if (currentLevel >= 250 && currentLevel <= 274)
                    badgeGlow->setColor({ 65, 239, 255 });
                if (currentLevel >= 275 && currentLevel <= 299)
                    badgeGlow->setColor({ 245, 108, 255 });
                
                auto fadein = CCEaseSineInOut::create(CCFadeTo::create(0.9f, 50));
                auto fadeout = CCEaseSineInOut::create(CCFadeTo::create(0.9f, 255));
                auto sequence = CCSequence::create(fadein, fadeout, nullptr);
                auto repeatSequence = CCRepeatForever::create(sequence);

                badgeGlow->runAction(repeatSequence);
            }

            return badgeSprite;
        }

        inline static CCSprite* getBadgeFromLevel(int level) {
            if (level >= 0 && level <= 4) {
                return CCSprite::createWithSpriteFrameName("tier0_0.png"_spr);
            }
            if (level >= 5 && level <= 9) {
                return CCSprite::createWithSpriteFrameName("tier0_1.png"_spr);
            }
            if (level >= 10 && level <= 14) {
                return CCSprite::createWithSpriteFrameName("tier0_2.png"_spr);
            }
            if (level >= 15 && level <= 19) {
                return CCSprite::createWithSpriteFrameName("tier0_3.png"_spr);
            }
            if (level >= 20 && level <= 24) {
                return CCSprite::createWithSpriteFrameName("tier0_4.png"_spr);
            }

            if (level >= 25 && level <= 29) {
                return CCSprite::createWithSpriteFrameName("tier1_0.png"_spr);
            }
            if (level >= 30 && level <= 34) {
                return CCSprite::createWithSpriteFrameName("tier1_1.png"_spr);
            }
            if (level >= 35 && level <= 39) {
                return CCSprite::createWithSpriteFrameName("tier1_2.png"_spr);
            }
            if (level >= 40 && level <= 44) {
                return CCSprite::createWithSpriteFrameName("tier1_3.png"_spr);
            }
            if (level >= 45 && level <= 49) {
                return CCSprite::createWithSpriteFrameName("tier1_4.png"_spr);
            }

            if (level >= 50 && level <= 54) {
                return CCSprite::createWithSpriteFrameName("tier2_0.png"_spr);
            }
            if (level >= 55 && level <= 59) {
                return CCSprite::createWithSpriteFrameName("tier2_1.png"_spr);
            }
            if (level >= 60 && level <= 64) {
                return CCSprite::createWithSpriteFrameName("tier2_2.png"_spr);
            }
            if (level >= 65 && level <= 69) {
                return CCSprite::createWithSpriteFrameName("tier2_3.png"_spr);
            }
            if (level >= 70 && level <= 74) {
                return CCSprite::createWithSpriteFrameName("tier2_4.png"_spr);
            }

            if (level >= 75 && level <= 79) {
                return CCSprite::createWithSpriteFrameName("tier3_0.png"_spr);
            }
            if (level >= 80 && level <= 84) {
                return CCSprite::createWithSpriteFrameName("tier3_1.png"_spr);
            }
            if (level >= 85 && level <= 89) {
                return CCSprite::createWithSpriteFrameName("tier3_2.png"_spr);
            }
            if (level >= 90 && level <= 94) {
                return CCSprite::createWithSpriteFrameName("tier3_3.png"_spr);
            }
            if (level >= 95 && level <= 99) {
                return CCSprite::createWithSpriteFrameName("tier3_4.png"_spr);
            }

            if (level >= 100 && level <= 104) {
                return CCSprite::createWithSpriteFrameName("tier4_0.png"_spr);
            }
            if (level >= 105 && level <= 109) {
                return CCSprite::createWithSpriteFrameName("tier4_1.png"_spr);
            }
            if (level >= 110 && level <= 114) {
                return CCSprite::createWithSpriteFrameName("tier4_2.png"_spr);
            }
            if (level >= 115 && level <= 119) {
                return CCSprite::createWithSpriteFrameName("tier4_3.png"_spr);
            }
            if (level >= 120 && level <= 124) {
                return CCSprite::createWithSpriteFrameName("tier4_4.png"_spr);
            }

            if (level >= 125 && level <= 129) {
                return CCSprite::createWithSpriteFrameName("tier5_0.png"_spr);
            }
            if (level >= 130 && level <= 134) {
                return CCSprite::createWithSpriteFrameName("tier5_1.png"_spr);
            }
            if (level >= 135 && level <= 139) {
                return CCSprite::createWithSpriteFrameName("tier5_2.png"_spr);
            }
            if (level >= 140 && level <= 144) {
                return CCSprite::createWithSpriteFrameName("tier5_3.png"_spr);
            }
            if (level >= 145 && level <= 149) {
                return CCSprite::createWithSpriteFrameName("tier5_4.png"_spr);
            }

            if (level >= 150 && level <= 154) {
                return CCSprite::createWithSpriteFrameName("tier6_0.png"_spr);
            }
            if (level >= 155 && level <= 159) {
                return CCSprite::createWithSpriteFrameName("tier6_1.png"_spr);
            }
            if (level >= 160 && level <= 164) {
                return CCSprite::createWithSpriteFrameName("tier6_2.png"_spr);
            }
            if (level >= 165 && level <= 169) {
                return CCSprite::createWithSpriteFrameName("tier6_3.png"_spr);
            }
            if (level >= 170 && level <= 174) {
                return CCSprite::createWithSpriteFrameName("tier6_4.png"_spr);
            }

            if (level >= 175 && level <= 179) {
                return CCSprite::createWithSpriteFrameName("tier7_0.png"_spr);
            }
            if (level >= 180 && level <= 184) {
                return CCSprite::createWithSpriteFrameName("tier7_1.png"_spr);
            }
            if (level >= 185 && level <= 189) {
                return CCSprite::createWithSpriteFrameName("tier7_2.png"_spr);
            }
            if (level >= 190 && level <= 194) {
                return CCSprite::createWithSpriteFrameName("tier7_3.png"_spr);
            }
            if (level >= 195 && level <= 199) {
                return CCSprite::createWithSpriteFrameName("tier7_4.png"_spr);
            }

            if (level >= 200 && level <= 204) {
                return CCSprite::createWithSpriteFrameName("tier8_0.png"_spr);
            }
            if (level >= 205 && level <= 209) {
                return CCSprite::createWithSpriteFrameName("tier8_1.png"_spr);
            }
            if (level >= 210 && level <= 214) {
                return CCSprite::createWithSpriteFrameName("tier8_2.png"_spr);
            }
            if (level >= 215 && level <= 219) {
                return CCSprite::createWithSpriteFrameName("tier8_3.png"_spr);
            }
            if (level >= 220 && level <= 224) {
                return CCSprite::createWithSpriteFrameName("tier8_4.png"_spr);
            }

            if (level >= 225 && level <= 229) {
                return CCSprite::createWithSpriteFrameName("tier9_0.png"_spr);
            }
            if (level >= 230 && level <= 234) {
                return CCSprite::createWithSpriteFrameName("tier9_1.png"_spr);
            }
            if (level >= 235 && level <= 239) {
                return CCSprite::createWithSpriteFrameName("tier9_2.png"_spr);
            }
            if (level >= 240 && level <= 244) {
                return CCSprite::createWithSpriteFrameName("tier9_3.png"_spr);
            }
            if (level >= 245 && level <= 249) {
                return CCSprite::createWithSpriteFrameName("tier9_4.png"_spr);
            }

            if (level >= 250 && level <= 254) {
                return CCSprite::createWithSpriteFrameName("tier10_0.png"_spr);
            }
            if (level >= 255 && level <= 259) {
                return CCSprite::createWithSpriteFrameName("tier10_1.png"_spr);
            }
            if (level >= 260 && level <= 264) {
                return CCSprite::createWithSpriteFrameName("tier10_2.png"_spr);
            }
            if (level >= 265 && level <= 269) {
                return CCSprite::createWithSpriteFrameName("tier10_3.png"_spr);
            }
            if (level >= 270 && level <= 274) {
                return CCSprite::createWithSpriteFrameName("tier10_4.png"_spr);
            }

            if (level >= 275 && level <= 279) {
                return CCSprite::createWithSpriteFrameName("tier11_0.png"_spr);
            }
            if (level >= 280 && level <= 284) {
                return CCSprite::createWithSpriteFrameName("tier11_1.png"_spr);
            }
            if (level >= 285 && level <= 289) {
                return CCSprite::createWithSpriteFrameName("tier11_2.png"_spr);
            }
            if (level >= 290 && level <= 294) {
                return CCSprite::createWithSpriteFrameName("tier11_3.png"_spr);
            }
            if (level >= 295 && level <= 299) {
                return CCSprite::createWithSpriteFrameName("tier11_4.png"_spr);
            }
            if (level >= 300 && level <= 349) {
                return CCSprite::createWithSpriteFrameName("tier12.png"_spr);
            }
            // TEMP
            return CCSprite::createWithSpriteFrameName("tier0_0.png"_spr);
        }

        inline static CCSprite* getGlowFromLevel(int level) {
            if (level >= 200 && level <= 204) {
                return CCSprite::createWithSpriteFrameName("glow_0.png"_spr);
            }
            if (level >= 205 && level <= 209) {
                return CCSprite::createWithSpriteFrameName("glow_1.png"_spr);
            }
            if (level >= 210 && level <= 214) {
                return CCSprite::createWithSpriteFrameName("glow_2.png"_spr);
            }
            if (level >= 215 && level <= 219) {
                return CCSprite::createWithSpriteFrameName("glow_3.png"_spr);
            }
            if (level >= 220 && level <= 224) {
                return CCSprite::createWithSpriteFrameName("glow_4.png"_spr);
            }

            if (level >= 225 && level <= 229) {
                return CCSprite::createWithSpriteFrameName("glow_0.png"_spr);
            }
            if (level >= 230 && level <= 234) {
                return CCSprite::createWithSpriteFrameName("glow_1.png"_spr);
            }
            if (level >= 235 && level <= 239) {
                return CCSprite::createWithSpriteFrameName("glow_2.png"_spr);
            }
            if (level >= 240 && level <= 244) {
                return CCSprite::createWithSpriteFrameName("glow_3.png"_spr);
            }
            if (level >= 245 && level <= 249) {
                return CCSprite::createWithSpriteFrameName("glow_4.png"_spr);
            }

            if (level >= 250 && level <= 254) {
                return CCSprite::createWithSpriteFrameName("glow_0.png"_spr);
            }
            if (level >= 255 && level <= 259) {
                return CCSprite::createWithSpriteFrameName("glow_1.png"_spr);
            }
            if (level >= 260 && level <= 264) {
                return CCSprite::createWithSpriteFrameName("glow_2.png"_spr);
            }
            if (level >= 265 && level <= 269) {
                return CCSprite::createWithSpriteFrameName("glow_3.png"_spr);
            }
            if (level >= 270 && level <= 274) {
                return CCSprite::createWithSpriteFrameName("glow_4.png"_spr);
            }

            if (level >= 275 && level <= 279) {
                return CCSprite::createWithSpriteFrameName("glow_0.png"_spr);
            }
            if (level >= 280 && level <= 284) {
                return CCSprite::createWithSpriteFrameName("glow_1.png"_spr);
            }
            if (level >= 285 && level <= 289) {
                return CCSprite::createWithSpriteFrameName("glow_2.png"_spr);
            }
            if (level >= 290 && level <= 294) {
                return CCSprite::createWithSpriteFrameName("glow_3.png"_spr);
            }
            if (level >= 295 && level <= 299) {
                return CCSprite::createWithSpriteFrameName("glow_4.png"_spr);
            }
            if (level >= 300 && level <= 349) {
                return CCSprite::createWithSpriteFrameName("glow_4.png"_spr);
            }

            return nullptr;
        }

        inline static CCSprite* getMostEXPIcon(GJUserScore* score) {
            int starEXP = score->m_stars * STAR_EXP;
            int moonEXP = score->m_moons * MOON_EXP;
            int diamondEXP = score->m_diamonds * DIAMOND_EXP;
            int secretCoinEXP = score->m_secretCoins * GOLD_COIN_EXP;
            int userCoinEXP = score->m_userCoins * USER_COIN_EXP;
            int demonEXP = score->m_demons * DEMON_EXP;
            int cpEXP = score->m_creatorPoints * CP_EXP;

            if (std::max({starEXP, moonEXP, diamondEXP, secretCoinEXP, userCoinEXP, demonEXP, cpEXP}) == starEXP)
                return CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
            if (std::max({starEXP, moonEXP, diamondEXP, secretCoinEXP, userCoinEXP, demonEXP, cpEXP}) == moonEXP)
                return CCSprite::createWithSpriteFrameName("GJ_moonsIcon_001.png");
            if (std::max({starEXP, moonEXP, diamondEXP, secretCoinEXP, userCoinEXP, demonEXP, cpEXP}) == diamondEXP)
                return CCSprite::createWithSpriteFrameName("GJ_diamondsIcon_001.png");
            if (std::max({starEXP, moonEXP, diamondEXP, secretCoinEXP, userCoinEXP, demonEXP, cpEXP}) == secretCoinEXP)
                return CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
            if (std::max({starEXP, moonEXP, diamondEXP, secretCoinEXP, userCoinEXP, demonEXP, cpEXP}) == userCoinEXP)
                return CCSprite::createWithSpriteFrameName("GJ_coinsIcon2_001.png");
            if (std::max({starEXP, moonEXP, diamondEXP, secretCoinEXP, userCoinEXP, demonEXP, cpEXP}) == demonEXP)
                return CCSprite::createWithSpriteFrameName("GJ_demonIcon_001.png");
            if (std::max({starEXP, moonEXP, diamondEXP, secretCoinEXP, userCoinEXP, demonEXP, cpEXP}) == cpEXP)
                return CCSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png");
            return nullptr;
        }

        inline static ccColor3B getColorFromLevel(int currentLevel) {
            ccColor3B barColor = {0, 0, 0};
            if (currentLevel >= 0 && currentLevel <= 24)
                barColor = { 156, 156, 156 };
            if (currentLevel >= 25 && currentLevel <= 49)
                barColor = { 228, 228, 228 };
            if (currentLevel >= 50 && currentLevel <= 74)
                barColor = { 135, 255, 145 };
            if (currentLevel >= 75 && currentLevel <= 99)
                barColor = { 83, 166, 255 };
            if (currentLevel >= 100 && currentLevel <= 124)
                barColor = { 234, 117, 255 };
            if (currentLevel >= 125 && currentLevel <= 149)
                barColor = { 255, 117, 177 };
            if (currentLevel >= 150 && currentLevel <= 174)
                barColor = { 255, 156, 117 };
            if (currentLevel >= 175 && currentLevel <= 199)
                barColor = { 255, 238, 106 };
            if (currentLevel >= 200 && currentLevel <= 224)
                barColor = { 255, 0, 89 };
            if (currentLevel >= 225 && currentLevel <= 249)
                barColor = { 145, 255, 0 };
            if (currentLevel >= 250 && currentLevel <= 274)
                barColor = { 0, 255, 255 };
            if (currentLevel >= 275)
                barColor = { 247, 0, 255 };
            if (currentLevel >= 300) {
                barColor = {255, 150, 150};
            }
            
            return barColor;
        }
};