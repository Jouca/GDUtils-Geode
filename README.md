# GDUtils

<img src="https://img.shields.io/github/downloads/Jouca/GDUtils-Geode/total?style=for-the-badge" style="text-align: center;"></img>
<a href="https://discord.gg/MU4Rpc6xbJ" style="text-align: center;"><img src="https://dcbadge.limes.pink/api/server/MU4Rpc6xbJ"></img></a>
<img src="https://img.shields.io/github/last-commit/Jouca/GDUtils-Geode?display_timestamp=committer&style=for-the-badge&color=ffA500" style="text-align: center;"></img>
<img src="https://img.shields.io/github/commit-activity/w/Jouca/GDUtils-Geode?style=for-the-badge" style="text-align: center;"></img>
<img src="https://img.shields.io/github/commits-since/Jouca/GDUtils-Geode/latest?style=for-the-badge" style="text-align: center;"></img>
<img src="https://img.shields.io/github/created-at/Jouca/GDUtils-Geode?style=for-the-badge" style="text-align: center;"></img>

**By [Jouca](https://github.com/Jouca) and [Firee](https://github.com/FireMario211)**
A Geometry Dash Mod for adding more tools such as Notifications, auto-song mute, game fixes & more!
![Mod Example](resources/Screenshot_1.png)

[![Ko-Fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/G2G711NJVI)
By supporting us on **Ko-Fi**, you contribute to **help developers** for their **work on the mod**, but also for the **rent of the server** we use for **notifications, URL redirection & more!**

**Extra perks for Membership (1.50$ / month)**:
* **GDUtils Supporter Badge** on GD Profile & Comments!
* **Discord role**!
* **Priority for suggesting ideas** on Discord server!

__**Join the Discord and Follow the instructions to Receive your In-Game Perks!**__

## Need help?
* You can join our [Discord server](https://discord.gg/MU4Rpc6xbJ) for **any assistance** or about **next updates**, **sneak peeks** & **more**!

# Features

### Realtime Notifications
* This includes any new levels and lists being **rated**, new **daily levels**, new **weekly levels!** and **new daily chests!**
* This also includes the new difficulty faces, and the new **Legendary** and **Mythic** rating.
* Notifications are **highly customizable**, you can set the __position__, __size__, and __how long they can stay on screen!__
* You can also set when notifications **should appear** or **not appear**! This includes __being shown in levels__, __in the editor__, __etc__.

### More Leaderboards
* Thanks to [Updated Leaderboards](https://discord.gg/HpC5Xc3JMh), it's now possible for you to view leaderboards for users with the most **Stars**, **Moons**, **Diamonds**, **User Coins**, **Demons!**, **Creator Points**, **Better Progression Levels (if mod installed)**, **Pointercrate** and **Classic/Platformers demons**! You can also view your accurate leaderboard placement.
* Leaderboards are now **infinite**! You can see the placement of every Geometry Dash players (registered with [Updated Leaderboards](https://discord.gg/HpC5Xc3JMh)) with this one.
* You can also see **leaderboards by countries**!

### Gauntlets redesign
* Gauntlets are now redesigned to make them more **EPIC**, with **particles** and **new textures**!

### New designs
* Some parts of Geometry Dash has been **redesigned** with new textures & stuffs!

### View Demon List Placement
* Instead of having to go to Pointercrate to view the demon placement of a level, you can now **view the position** of an extreme demon!

### Level URL Sharing Feature (Windows Only)
* You can share levels easily by copying the URL of it and sending it to your friends! Or you can quickly go to a level by going to `https://gdutils.com/{levelid}` (Replacing `{levelid}` with the ID of the level)
* With the compatility of **Thumbnail Mod**, you will be able to see it by sending it on **Discord**!

### Moderator List 
* You can view the list of **Elder Moderators**/**Moderators**/**Leaderboard Moderators** and their socials!

### Auto-Mute Spotify (or any application)
* Allows you to **auto mute Spotify** or any other applications when you are __playing a level__, __playtesting in the editor__, and more!
* **IMPORTANT for Mac Users:** Geometry Dash may request access to "System Events" and another app for muting Spotify or another application. This allows the game to detect running applications.

### Custom backgrounds
* Custom your **backgrounds colors** or you can even make it **transparent**!

### Extra additions to Friends List
* The additions added include being able to **search usernames** if you have a long friend list, or using the **scroll buttons**!

### Moderator Info
* You can view a description of either the Moderator, Elder Moderator, or other badges users have by clicking on the badge itself on the profile!

## API Usage
Incase you want to utilize GDUtils for your Geode Mod, you can do so by using it's events!

### Events 
- `RateEvent` - For other mods wanting to "notify" the user through GDUtils. (You shouldn't use this as a listener)
- `OnRate` - For checking when GDUtils gives out a rate notification. (You should use this only as a listener)

### Example Code (Pushing out a notification)
```cpp
EventData data = {
    true, // demon
    2, // starsum
    6, // stars
    1, // rate (0 = unrated, 1 = featured, 2 = epic, 3 = legendary, 4 = mythic)
    EventType::Rate, // type of notification
    "Custom Notification!", // notification title
    "GJ_square02.png", // sprite (MUST BE VALID OR WILL CRASH!)
    "Tidal Wave", // level name
    "By OniLinkGD", // level creator
    0, // coins
    false, // whether coins are verified
    false // if its platformer
}; // Level ID is optional
GDUtils::Events::RateEvent::emit(data);
```

### Example Code (Listening for notification events from GDUtils)
```cpp
$execute {
    new EventListener<EventFilter<GDUtils::Events::OnRate>>(+[](GDUtils::Events::OnRate* e) {
        log::info("A rate event with the title {}", e->getTitle()); // A rate event with the title Small Daily Chest available!
        return ListenerResult::Propagate;
    });
}
```
View the `test` directory if you want to view an example mod using the API.

## Libraries Used
- [Geode](https://github.com/geode-sdk/geode)
- [socket.io-client-cpp](https://github.com/socketio/socket.io-client-cpp)
- [pl_mpeg](https://github.com/phoboslab/pl_mpeg)

## Credits
* [Jouca](https://twitter.com/JoucaJouca)
* [Firee](https://youtube.com/@gdfiree)
* [nekitdev](https://twitter.com/nekitdev)
* [Maverick](https://www.youtube.com/channel/UCoFBsXJ-6o6l6ZYP-k6DE_g)
* [xvia](https://x.com/SZidnes)
* [brift](https://x.com/BriftXD)

**Thank you to RobTopGames for his trust on the mod.**

## License
This project is created by [Jouca](https://github.com/Jouca) under the [GNU General Public License v3.0](https://choosealicense.com/licenses/gpl-3.0/), read more by clicking on the highlighted name.
