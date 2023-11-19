#include "Discord.h"
#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
using namespace geode::prelude;

// why is there barely any documentation

static const char* APPLICATION_ID = "1173428842564702353";

static int64_t StartTime;

void Discord::init() {
    DiscordEventHandlers handlers{};

    // no discord, im not making separate functtions, imma do lambdas instead
    handlers.ready = [](const DiscordUser* connectedUser) {
        log::info("Discord RPC Connected!");
    };
    handlers.disconnected = [](int errcode, const char* message) {
        log::warn(fmt::format("Discord RPC Disconnected. ({}: {})", errcode, message));
    };
    handlers.errored = [](int errcode, const char* message) {
         log::error(fmt::format("Discord RPC Error. ({}: {})", errcode, message));

    };
    //handlers.spectateGame = handleDiscordSpectate;
    Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
    Discord_RunCallbacks();

}

void Discord::update(const char* layerName) {
    auto gm = GameManager::sharedState();
    DiscordRichPresence discordPresence{};
    discordPresence.details = "Browsing Menus";
    discordPresence.state = "Test";
    discordPresence.startTimestamp = StartTime;
    discordPresence.largeImageKey = "gd_icon";
    discordPresence.largeImageText = gm->m_playerName.c_str();
    discordPresence.instance = 0;
    Discord_UpdatePresence(&discordPresence);
}
