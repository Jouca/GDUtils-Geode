#pragma once
// windows only because i cant get this to work, actually im too lazy
#ifdef GEODE_IS_WINDOWS
#include <Geode/Geode.hpp>
#include "BrownAlertDelegate.hpp"
#include <fstream>
#include <curlmod/curl.h> // listen here devs, i know what you're going to say, USE GEODE WEB REQUESTS INSTEAD
                          // but listen, if you look more into the code, youll see i have a progress bar 
                          // geode web requests doesnt have a progress bar......
struct HttpResponse {
    std::string header;
    std::string content;
    CURLcode curlCode;
    long responseCode;
};
class ProgressBar : public CCNode {
    protected:
        float m_pPercentage = 0;
        CCSprite* outerBg = CCSprite::create("slidergroove.png");
        CCSprite* progressBar = CCSprite::create("sliderBar.png");
        bool init();
    public:
        void testProgressbar();
        virtual void setProgress(float percentage);
        static ProgressBar* create();
};
class DownloadManager : public BrownAlertDelegate {
    LoadingCircle* loading_circle = LoadingCircle::create();

    // this is why i dont like threads, cocos2d cant handle cclabels so i have to do this
    // also success description may need to change because im unsure if youre going to use DownloadManager to download other things
    FLAlertLayer* error_box1 = FLAlertLayer::create(nullptr, "Error!", "An error occured while trying to send a request to the servers.", "OK", nullptr, 200.0F);
    FLAlertLayer* error_box2 = FLAlertLayer::create(nullptr, "Error!", "An error occured while to write the file!\nPlease ensure that you have enough disk space.", "OK", nullptr, 300.0F);
    protected:
        virtual void setup();
        cocos2d::CCSize m_sScrLayerSize;
        const char* m_sUrl;
        const char* m_sDestination;
        float m_fWidth;
        float m_fHeight;
        cocos2d::SEL_MenuHandler m_pSelector;

        /**
         * CURL helper functions
        */
        static size_t writeData(void *ptr, size_t size, size_t nmemb, std::string* data) {
            data->append((char*) ptr, size * nmemb);
            return size * nmemb;
        }
        HttpResponse sendWebRequest(const std::string& url) {
            auto curl = curl_easy_init();
            if (!curl) return {"", "", CURLE_FAILED_INIT, 0};
            HttpResponse ret;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(ret.content));
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &(ret.header));
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
            curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
            curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, TRUE);
            ret.curlCode = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &(ret.responseCode));
            if(ret.content.size() == 0) {
                ret.curlCode = CURLE_HTTP_RETURNED_ERROR;
            }
            if(ret.content.rfind("<html") == 0) {
                ret.curlCode = CURLE_HTTP_RETURNED_ERROR;
            }
            if(ret.content.rfind("<!DOCT", 0) == 0) {
                ret.curlCode = CURLE_HTTP_RETURNED_ERROR;
            }
            curl_easy_cleanup(curl);
            return ret;
        }
    public:
        static constexpr const float s_defWidth = 300.0f;
        static constexpr const float s_defHeight = 180.0f;
        static DownloadManager* create(const char* url, const char* destination, cocos2d::SEL_MenuHandler callback); //, cocos2d::SEL_MenuHandler callback
        
        // Downloader class
        static int progress_func(void*, double, double, double, double);
        bool dumpToFile(const std::string& path, std::string data) {
            std::ofstream fout(path, std::ios::out | std::ios::binary);
            fout.write(data.c_str(), data.size());
            fout.close();
            if (!fout) return false;
            return true;
        }
        static DWORD WINAPI curl_thread(void*);
};
#endif
