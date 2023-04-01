#include <osw_ui.h>

#include <OswAppDrawer.h>

bool OswAppDrawer::minimizeButtonLabels = false;

OswAppDrawer::OswAppDrawer(size_t* sleepPersistantAppIndex): sleepPersistantAppIndex(sleepPersistantAppIndex) {
    // nothing to do here
}

void OswAppDrawer::setDefault(const char* defaultAppId) {
    this->defaultAppId = defaultAppId;
}

const char* OswAppDrawer::getAppId() {
    return "osw.drawer";
}

const char* OswAppDrawer::getAppName() {
    return "OSW Drawer";
}

void OswAppDrawer::registerApp(const char* category, OswAppV2* app) {
    if(!this->apps.count(category)) // if the category does not exist yet, create it
        this->apps.emplace(std::make_pair(category, std::move(std::list<LazyInit>())));
    this->apps.at(category).emplace_back(app);
}

void OswAppDrawer::onStart() {
    OswAppV2::onStart();
    this->viewFlags = (OswAppV2::ViewFlags)(this->viewFlags | OswAppV2::ViewFlags::KEEP_DISPLAY_ON); // in the drawer, we want to keep the display on
    // the known button states are handled with drawer/app open/close
    
    // Try to find it the default app...
    LazyInit* defaultApp = nullptr;
    if(this->sleepPersistantAppIndex != nullptr and *this->sleepPersistantAppIndex != UNDEFINED_SLEEP_APP_INDEX) {
        // Using the sleep-persistant app index, we are maybe able to recover the last app
        size_t categoryIndex = 0;
        for(auto& category: this->apps) {
            for(auto& app: category.second) {
                if(categoryIndex == *this->sleepPersistantAppIndex) {
                    defaultApp = &app;
                    OSW_LOG_D("Selected default app based on sleep persistent index: ", *this->sleepPersistantAppIndex);
                    break;
                }
                ++categoryIndex;
            }
            if(defaultApp != nullptr)
                break;
        }
    }
    
    // Either the previous step failed, or we did not have a sleep-persistant app index
    if(defaultApp == nullptr) {
        // Maybe a default category / index was set?
        if(this->defaultAppId) {
            std::string defaultAppId = this->defaultAppId; // using a string here, as we need to compare it to the app id
            for(auto& category: this->apps) {
                for(auto& app: category.second) {
                    if(app.get()->getAppId() == defaultAppId) {
                        defaultApp = &app;
                        OSW_LOG_D("Selected default app based on app id: \"", this->defaultAppId, "\"");
                        break;
                    }
                }
                if(defaultApp != nullptr)
                    break;
            }
        } else {
        }
    }

    // Okay, let's hope we can select the first app in the "first" category (sorted based on hash)
    if(defaultApp == nullptr and this->apps.size() > 0) {
        // Well, guess we have to take the "first" then
        defaultApp = &this->apps.begin()->second.front();
        OSW_LOG_D("Selected default app based who came \"first\"...");
    }

    assert(defaultApp != nullptr && "Drawer failed to determine default app (this should never happen) - or is the drawer empty?");
    this->open(*defaultApp);
}

void OswAppDrawer::onLoop() {
    OswAppV2::onLoop(); // needed to detect long-press of menu-select / navigation with drawer open
    if(this->current)
        this->current->get()->onLoop(); // if we are not in the drawer, we want to call the current app's onLoop
}

void OswAppDrawer::onDraw() {
    if(!this->current) {
        OswAppV2::onDraw();
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
        char const* highlightedAppName = nullptr;
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
                    highlightedAppName = app.get()->getAppName();
                    highlightApp = &app;
                }
                // draw the app icon
                const float iconScale = horizontalAppSize / app.get()->getAppIcon().baseDimensions;
                app.get()->getAppIcon().draw(this->hal->gfx(), currentCategoryWidth + horizontalAppSize / 2, currentCategoryHeightStart + horizontalAppSize / 2, iconScale, OswImage::Alignment::CENTER, OswImage::Alignment::CENTER);
                ++categoryAppIndex;
                if(categoryAppIndex > (horizontalAppCount - 1))
                    break;
            }
            ++categoryIndex;
            if(categoryIndex > (categoryCount - 1))
                break;
        }

        // draw the selected app name
        if(highlightedAppName) {
            this->hal->gfx()->setTextCursor(DISP_W / 2, DISP_H - 16);
            this->hal->gfx()->setTextCenterAligned();
            this->hal->gfx()->print(highlightedAppName);
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
        this->current->get()->onDraw();
}

void OswAppDrawer::onDrawOverlay() {
    if(this->current)
        this->current->get()->onDrawOverlay(); // forward to the current app
    OswAppV2::onDrawOverlay(); // always needed to draw e.g. the animation for the menu button
}

void OswAppDrawer::onStop() {
    if(this->current)
        this->current->get()->onStop(); // forward to the current app
    OswAppV2::onStop();
}

void OswAppDrawer::onButton(int id, bool up, ButtonStateNames state) {
    OswAppV2::onButton(id, up, state);
    if(!this->current) {
        if(up and state == OswAppV2::ButtonStateNames::SHORT_PRESS) {
            if(id == Button::BUTTON_UP) {
                this->highlightCategoryIndex = (this->highlightCategoryIndex + 1) % this->apps.size();
                this->needsRedraw = true;
            } else if(id == Button::BUTTON_DOWN) {
                ++this->highlightAppIndex;
                this->needsRedraw = true;
            } else if(id == Button::BUTTON_SELECT) {
                this->open(*highlightApp);
            }
        }
        this->minimizeButtonLabels = true; // any button press will minimize the button labels
    } else {
        if(up == true and id == Button::BUTTON_SELECT and state == OswAppV2::ButtonStateNames::LONG_PRESS) {
            this->drawer();
        }
        // Do not forward the button press to the current app if the drawer is open
    }
}

#ifdef OSW_EMULATOR
void OswAppDrawer::onLoopDebug() {
    if(this->current)
        this->current->get()->onLoopDebug(); // forward to the current app
    else
        OswAppV2::onLoopDebug();
}
#endif

const OswAppV2::ViewFlags& OswAppDrawer::getViewFlags() {
    if(this->current)
        return this->current->get()->getViewFlags(); // forward to the current app
    else
        return OswAppV2::getViewFlags();
}

bool OswAppDrawer::getNeedsRedraw() {
    if(this->current)
        return OswAppV2::getNeedsRedraw() or this->current->get()->getNeedsRedraw(); // still respect the redraw of the drawers overlays
    else
        return OswAppV2::getNeedsRedraw();
}

void OswAppDrawer::resetNeedsRedraw() {
    OswAppV2::resetNeedsRedraw();
    if(this->current)
        this->current->get()->resetNeedsRedraw(); // forward to the current app
}

/**
 * @brief This destorys all cached app instances, leaving only the current app running.
 * 
 */
void OswAppDrawer::cleanup() {
    for(auto categoryIterator = this->apps.begin(); categoryIterator != this->apps.end(); categoryIterator++) {
        for(auto appIterator = categoryIterator->second.begin(); appIterator != categoryIterator->second.end(); appIterator++) {
            if(this->current and *appIterator != this->current->get()) {
                appIterator->cleanup();
            }
        }
    }
}

void OswAppDrawer::drawer() {
    if(!this->current)
        return;
    this->current->get()->onStop();
    this->current = nullptr;
    this->clearKnownButtonStates(false);
    this->knownButtonStates[Button::BUTTON_UP] = ButtonStateNames::SHORT_PRESS;
    this->knownButtonStates[Button::BUTTON_DOWN] = ButtonStateNames::SHORT_PRESS;
    this->knownButtonStates[Button::BUTTON_SELECT] = ButtonStateNames::SHORT_PRESS;
    this->needsRedraw = true;
}

void OswAppDrawer::open(LazyInit& app) {
    this->drawer(); // stop current app (by "opening" the drawer), ignores if drawer is already open
    this->clearKnownButtonStates(false);
    this->knownButtonStates[Button::BUTTON_SELECT] = ButtonStateNames::LONG_PRESS;

    // start app
    this->current = &app;
    this->current->get()->onStart();
    this->cleanup(); // cleanup all other apps

    // store the selected app "index" persistently, if variable was set
    if(this->sleepPersistantAppIndex != nullptr) {
        size_t index = 0;
        bool found = false;
        for(auto& category : this->apps) {
            for(auto& catApp : category.second) {
                if(catApp == *this->current) {
                    *this->sleepPersistantAppIndex = index;
                    found = true;
                    OSW_LOG_D("Stored sleep-persistant app index: ", index);
                    break;
                }
                ++index;
            }
            if(found)
                break;
        }
        assert(found && "Could not find the current app in the app drawer?!"); // this should never happen
    }
}