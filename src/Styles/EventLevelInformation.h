#pragma once
#include "../includes.h"
#include "Geode/binding/FLAlertLayer.hpp"
#include "../OldBorder.hpp"

class EventLevelInformation : public Popup<> {
    protected:
        bool setup() override;
        void loadPage();

    public:
        static void scene();
};