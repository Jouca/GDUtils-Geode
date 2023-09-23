#ifndef _VIDEO_PLAYER_HPP
#define _VIDEO_PLAYER_HPP

#pragma warning(disable:4996)
#include "pl_mpeg.h"
#pragma warning(default:4996)

#include <queue>
#include "filesystem.hpp"
#include "fmod.hpp"
#include <Geode/Geode.hpp>
#include <Geode/fmod/fmod.h>

typedef unsigned int GLuint;


namespace videoplayer {
    class VideoPlayer : public cocos2d::CCNodeRGBA {
    protected:
        bool init(ghc::filesystem::path const& path, bool loop);
        
        void initAudio();
        static FMOD_RESULT F_CALLBACK audioCallback(FMOD_CHANNELCONTROL *chanControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *commandData1, void *commandData2);

        virtual void update(float delta) override;
        virtual void draw() override;

        virtual ~VideoPlayer();
        virtual void onExit() override;

        static void videoCallback(plm_t* mpeg, plm_frame_t* frame, void* user);
        static void audioCallback(plm_t* mpeg, plm_samples_t* samples, void* user);

        static FMOD_RESULT F_CALLBACK PCMRead(FMOD_SOUND *sound, void *data, unsigned int length);

        ghc::filesystem::path m_path;
        plm_t* m_stream;

        cocos2d::CCSize m_dimensions;
        GLuint m_textures[3]; // y, cb, cr
        std::queue<float> m_samples;

        FMOD::Channel* m_channel;
        FMOD::Sound* m_sound;

        bool m_paused;
        bool m_loop;
        bool m_stopped;
        float m_volume = 1.0f;
    public:

        /**
         * @brief Allocates and initializes a video player.
         * 
         * @param path Path to the video file (Could use Mod::get()->getResourcesDir() / "file.mpg").
         * @param loop Whether or not playback should loop upon completion.
         * @return A new initialized video player
         */
        static VideoPlayer* create(ghc::filesystem::path const& path, bool loop=false);

        /**
         * @brief Sets the content height of the video player, maintaining aspect ratio
         * 
         * @param height The new content height for the video player
         */
        void setHeight(float height);

        /**
         * @brief Sets the content width of the video player, maintaining aspect ratio
         * 
         * @param width The new content width for the video player
         */
        void setWidth(float width);

        /**
         * @brief Modifies the content size of the video player to fit a given size, maintaining aspect ratio
         * 
         * @param size The size to fill
         */
        void fillSize(cocos2d::CCSize size);

        /**
         * @brief Sets the volume of playback.
         * W
         * @param volume The new volume
         */
        void setVolume(float volume);

        /**
         * @brief Pauses playback
         * 
         */
        void pause();

        /**
         * @brief Resumes playback
         * 
         */
        void resume();

        /**
         * @brief Toggles playback.
         * 
         */
        void toggle();

        /**
         * @brief Returns whether playback is paused.
         * 
         * @return The playback status
         */
        bool isPaused();
    };
}

#endif
