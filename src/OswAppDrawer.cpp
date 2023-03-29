#include <osw_ui.h>

#include <OswAppDrawer.h>

bool OswAppDrawer::minimizeButtonLabels = false;

OswAppDrawer::OswAppDrawer(const char* defaultCategory, size_t defaultAppIndex): defaultCategory(defaultCategory), defaultAppIndex(defaultAppIndex) {
    // nothing to do here
}

const char* OswAppDrawer::getAppId() {
    return "osw.drawer";
}

const char* OswAppDrawer::getAppName() {
    return "OSW Drawer";
}

void OswAppDrawer::registerApp(const char* category, OswAppV2* app) {
    if(!this->apps.count(category)) // if the category does not exist yet, create it
        this->apps.insert({category, {}});
    this->apps.at(category).push_back({app});
}

OswAppDrawer::LazyInit& OswAppDrawer::getCurrent() {
    if(this->current == nullptr) {
        // If the current app is not set, we search and set the default app
        std::list<LazyInit>* categoryList;
        if(this->defaultCategory) {
            if(!this->apps.count(this->defaultCategory))
                throw std::runtime_error("Invalid default category");
            categoryList = &this->apps.at(this->defaultCategory);
        } else {
            // well, guess we have to take the first category then
            categoryList = &this->apps.begin()->second;
        }
        if(categoryList->size() <= this->defaultAppIndex)
            throw std::runtime_error("Invalid default app index");
        auto it = categoryList->begin();
        std::advance(it, this->defaultAppIndex);
        this->current = &*it;
    }
    return *this->current;
}

void OswAppDrawer::onStart() {
    OswAppV2::onStart();
    if(this->showDrawer)
        this->viewFlags = (OswAppV2::ViewFlags)(this->viewFlags | OswAppV2::ViewFlags::KEEP_DISPLAY_ON); // in the drawer, we want to keep the display on
    else
        this->getCurrent()->onStart();
}

void OswAppDrawer::onLoop() {
    OswAppV2::onLoop();
    if(!this->showDrawer)
        this->getCurrent()->onLoop(); // if we are not in the drawer, we want to call the current app's onLoop
}

void OswAppDrawer::onDraw() {
    OswAppV2::onDraw();
    if(this->showDrawer) {
        this->hal->gfx()->setTextCursor(DISP_W / 2, DISP_H / 2);

        // some constants for the drawer layout
        const int categoryHeightStart = 48;
        const int categoryHeightPadding = 8; // used for the category texts and their horizontal divider lines
        const int categoryWidthStart = 40;
        const int categoryWidth = DISP_W - 2 * categoryWidthStart;
        const int horizontalAppCount = 3;
        const int categoryCount = 3;
        const int horizontalAppSize = (categoryWidth - horizontalAppCount * categoryHeightPadding) / horizontalAppCount;

        // handle (over-)scrolling
        if(this->highlightCategoryIndex > (categoryCount - 1)) {
            // if we scrolled over the display end -> advance the offset
            --this->highlightCategoryIndex;
            ++this->categoryIndexOffset;
        }
        if(this->categoryIndexOffset + this->highlightCategoryIndex >= this->apps.size()) {
            // if we scrolled over the end -> reset to the beginning
            this->highlightCategoryIndex = 0;
            this->categoryIndexOffset = 0;
        }

        // now draw all the categories
        char const* selectedAppName = nullptr;
        size_t categoryIndex = 0;
        auto categoryIterator = this->apps.begin();
        std::advance(categoryIterator, this->categoryIndexOffset);
        for(; categoryIterator != this->apps.end(); categoryIterator++) {
            auto& categoryName = categoryIterator->first;
            auto& categoryApps = categoryIterator->second;
            const int currentCategoryHeightStart = categoryHeightStart + (horizontalAppSize + categoryHeightPadding) * categoryIndex;
            // draw the category name
            this->hal->gfx()->setTextCursor(categoryWidthStart, currentCategoryHeightStart);
            this->hal->gfx()->setTextColor(OswUI::getInstance()->getForegroundColor());
            this->hal->gfx()->print(categoryName);
            // calc the app page
            if(categoryIndex == this->highlightCategoryIndex and this->highlightAppIndex > categoryApps.size() - 1)
                this->highlightAppIndex = 0;
            const size_t categoryAppPage = (categoryIndex == this->highlightCategoryIndex) ? this->highlightAppIndex / horizontalAppCount : 0;
            // if needed, print the page number (and maximum)
            if(categoryApps.size() > horizontalAppCount) {
                this->hal->gfx()->print(" ");
                this->hal->gfx()->print(String(categoryAppPage + 1));
                this->hal->gfx()->print("/");
                this->hal->gfx()->print(String((int) std::ceil((float) categoryApps.size() / horizontalAppCount)));
            }
            // draw the horizontal divider line - starting at the end of the category name (and potential page number)
            this->hal->gfx()->drawHLine(this->hal->gfx()->getTextCursorX(), currentCategoryHeightStart - 3, categoryWidth - (this->hal->gfx()->getTextCursorX() - categoryWidthStart), OswUI::getInstance()->getForegroundDimmedColor());
            // draw the apps
            size_t categoryAppIndex = 0;
            auto appIterator = categoryApps.begin();
            std::advance(appIterator, categoryAppPage * horizontalAppCount);
            for(; appIterator != categoryApps.end(); appIterator++) {
                auto& app = *appIterator;
                const int currentCategoryWidth = categoryWidthStart + (horizontalAppSize + categoryHeightPadding) * categoryAppIndex;
                // draw a frame about the selected app (and store refs to the app and its name)
                if(categoryAppIndex == (this->highlightAppIndex % horizontalAppCount) and categoryIndex == this->highlightCategoryIndex) {
                    this->hal->gfx()->drawFrame(currentCategoryWidth, currentCategoryHeightStart, horizontalAppSize, horizontalAppSize, OswUI::getInstance()->getInfoColor());
                    selectedAppName = app->getAppName();
                    highlightApp = &app;
                }
                // draw the app icon
                const float iconScale = horizontalAppSize / app->getAppIcon().baseDimensions;
                app->getAppIcon().draw(this->hal->gfx(), currentCategoryWidth + horizontalAppSize / 2, currentCategoryHeightStart + horizontalAppSize / 2, iconScale, OswImage::Alignment::CENTER, OswImage::Alignment::CENTER);
                ++categoryAppIndex;
                if(categoryAppIndex > (horizontalAppCount - 1))
                    break;
            }
            ++categoryIndex;
            if(categoryIndex > (categoryCount - 1))
                break;
        }

        // draw the selected app name
        if(selectedAppName) {
            this->hal->gfx()->setTextCursor(DISP_W / 2, DISP_H - 16);
            this->hal->gfx()->setTextCenterAligned();
            this->hal->gfx()->print(selectedAppName);
        }

        // draw the button labels
        if(!this->minimizeButtonLabels) {
            OswUI::getInstance()->setTextCursor(Button::BUTTON_UP);
            this->hal->gfx()->print("Category");

            OswUI::getInstance()->setTextCursor(Button::BUTTON_DOWN);
            this->hal->gfx()->print("App");

            OswUI::getInstance()->setTextCursor(Button::BUTTON_SELECT);
        } else {
            OswUI::getInstance()->setTextCursor(Button::BUTTON_SELECT);
            this->hal->gfx()->setTextSize(1);
        }
        this->hal->gfx()->print("OK");
    } else
        this->getCurrent()->onDraw();
}

void OswAppDrawer::onDrawOverlay() {
    if(!this->showDrawer)
        this->getCurrent()->onDrawOverlay(); // forward to the current app
    OswAppV2::onDrawOverlay();
}

void OswAppDrawer::onStop() {
    OswAppV2::onStop();
    if(!this->showDrawer)
        this->getCurrent()->onStop(); // forward to the current app
}

void OswAppDrawer::onButton(int id, bool up, ButtonStateNames state) {
    OswAppV2::onButton(id, up, state);
    if(this->showDrawer) {
        if(up and state == OswAppV2::ButtonStateNames::SHORT_PRESS) {
            if(id == Button::BUTTON_UP) {
                this->highlightCategoryIndex = (this->highlightCategoryIndex + 1) % this->apps.size();
                this->needsRedraw = true;
            } else if(id == Button::BUTTON_DOWN) {
                ++this->highlightAppIndex;
                this->needsRedraw = true;
            } else if(id == Button::BUTTON_SELECT) {
                this->onStop();
                this->showDrawer = false;
                this->current = highlightApp;
                this->getCurrent()->onStart();
                this->needsRedraw = true;
                this->cleanup();
            }
        }
        this->minimizeButtonLabels = true; // any button press will minimize the button labels
    } else {
        if(up == true and id == Button::BUTTON_SELECT and state == OswAppV2::ButtonStateNames::LONG_PRESS) {
            this->showDrawer = true;
            this->getCurrent()->onStop();
            this->current = nullptr;
            this->onStart(); // "start" the drawer app
        } else
            this->getCurrent()->onButton(id, up, state);
    }
}

#ifdef OSW_EMULATOR
void OswAppDrawer::onLoopDebug() {
    OswAppV2::onLoopDebug();
    if(!this->showDrawer)
        this->getCurrent()->onLoopDebug(); // forward to the current app
}
#endif

OswAppV2::ViewFlags OswAppDrawer::getViewFlags() {
    if(this->showDrawer)
        return OswAppV2::getViewFlags();
    else
        return this->getCurrent()->getViewFlags(); // forward to the current app
}

bool OswAppDrawer::getNeedsRedraw() {
    if(this->showDrawer)
        return OswAppV2::getNeedsRedraw();
    else
        return OswAppV2::getNeedsRedraw() or this->getCurrent()->getNeedsRedraw(); // still respect the redraw of the drawers overlays
}

void OswAppDrawer::resetNeedsRedraw() {
    OswAppV2::resetNeedsRedraw();
    if(!this->showDrawer)
        this->getCurrent()->resetNeedsRedraw(); // forward to the current app
}

/**
 * @brief This destorys all cached app instances, leaving only the current app running.
 * 
 */
void OswAppDrawer::cleanup() {
    for(auto categoryIterator = this->apps.begin(); categoryIterator != this->apps.end(); categoryIterator++) {
        for(auto appIterator = categoryIterator->second.begin(); appIterator != categoryIterator->second.end(); appIterator++) {
            if(*appIterator != *this->current) {
                appIterator->cleanup();
            }
        }
    }
}