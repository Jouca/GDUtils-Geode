#pragma once
#include "../includes.h"
#include "Geode/binding/FLAlertLayer.hpp"
#include "../OldBorder.hpp"

#include <Geode/utils/web.hpp>

class EventLevelInformation : public Popup<> {
    protected:
        EventListener<web::WebTask> m_downloadListener;
        Ref<CCImage> m_image;
        LoadingCircle* loading_circle = nullptr;

        bool setup() override;
        void loadPage();
        void imageCreationFinished(CCImage* image);
        void onDownloadFail();
        void fadeLoadingCircle();

    public:
        static void scene();
};