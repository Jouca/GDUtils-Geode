#include "SelectRegionCell.h"

bool SelectRegionCell::init(std::string mod) {
    std::vector<std::string> data = SelectRegion::getWords(mod, ":");
    std::vector<std::string> data2 = data;

    while (data.size() > 0) {
        std::string id = data[0];
        std::string name = data[1];

        // sprite

        data.erase(data.begin());
        data.erase(data.begin());
    }

    return true;
};

SelectRegionCell* SelectRegionCell::create(std::string mod) {
    auto ret = new SelectRegionCell();
    if (ret && ret->init(mod)) {
        ret->retain();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}
