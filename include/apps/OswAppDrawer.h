#pragma once
#include <map>
#include <list>
#include <utility>
#include <string_view>

#include <OswIcon.h>
#include <OswAppV2.h>

class OswAppDrawer: public OswAppV2 {
  public:
    static const size_t UNDEFINED_SLEEP_APP_INDEX = (size_t) -1; // just use a very large number

    OswAppDrawer(size_t* sleepPersistantAppIndex = nullptr);

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onDrawOverlay() override;
    void onStop() override;

    void onButton(Button id, bool up, ButtonStateNames state) override;
#ifdef OSW_EMULATOR
    void onLoopDebug() override;
#endif

    const ViewFlags& getViewFlags() override;
    bool getNeedsRedraw() override;
    void resetNeedsRedraw() override;

    void registerApp(const char* category, OswAppV2* app);
    template<typename T>
    void registerAppLazy(const char* category) {
        if(!this->apps.count(category))
            this->apps.emplace(std::make_pair(category, std::move(std::list<LazyInit>())));
        this->apps.at(category).emplace_back(nullptr);
        this->apps.at(category).back().set<T>();
    };
#ifdef OSW_EMULATOR
    void reset();
#endif

    // Control functions, those will schedule their action for the next loop() of the drawer (preventing e.g. undefined behavior of switching apps while drawing)
    void showDrawer();
    void startApp(const char* appId);
  private:
    class LazyInit {
      public:
        LazyInit(OswAppV2* given) {
            this->ptr = given;
        };
        LazyInit(LazyInit& other) = delete; // prevent copying, which may causes issues with the cleanup

        template<typename T>
        void set() {
            this->init = []() -> OswAppV2* {
                return new T();
            };
        }

        OswAppV2* get() {
            if(this->ptr == nullptr)
                this->ptr = this->init();
            return this->ptr;
        }

        bool operator==(const LazyInit& other) const {
            // either the app-instance is the same, or the initialization function is the same
            return (this->ptr != nullptr and other.ptr != nullptr and this->ptr == other.ptr) or (this->init != nullptr and other.init != nullptr and this->init == other.init);
        }

        bool operator!=(const LazyInit& other) const {
            return not (*this == other);
        }

        void cleanup() {
            if(this->init != nullptr) {
                delete this->ptr;
                this->ptr = nullptr;
            }
        }

        virtual ~LazyInit() {
            this->cleanup();
        }
      private:
        OswAppV2* (*init)() = nullptr;
        OswAppV2* ptr = nullptr;
    };
    static bool minimizeButtonLabels; // if you know one drawer, you know them all ;)
    std::map<std::string_view, std::list<LazyInit>> apps; // using string view for correct ordering & comparison
    LazyInit* current = nullptr;
    size_t highlightCategoryIndex = 0;
    size_t highlightAppIndex = 0;
    size_t categoryIndexOffset = 0;
    LazyInit* highlightApp = nullptr;
    size_t* sleepPersistantAppIndex;

    OswAppDrawer::LazyInit* nextLoopAppOpen = nullptr;
    bool nextLoopDrawerOpen = false;

    void cleanup();
    void drawer();
    void open(LazyInit& app);

    // For UI testing purposes (to access private member "apps")
    // IMPORTANT: declaring such friend classes are the only changes in production code for testing purposes
    friend class TestDrawer;
};
