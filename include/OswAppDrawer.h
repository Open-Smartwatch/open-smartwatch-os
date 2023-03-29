#pragma once
#include <map>
#include <list>

#include <OswIcon.h>
#include <OswAppV2.h>

class OswAppDrawer: public OswAppV2 {
  public:
    OswAppDrawer(const char* defaultCategory = nullptr, size_t defaultAppIndex = 0);
  
    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onDrawOverlay() override;
    void onStop() override;

    void onButton(int id, bool up, ButtonStateNames state) override;
#ifdef OSW_EMULATOR
    void onLoopDebug() override;
#endif

    ViewFlags getViewFlags() override;
    bool getNeedsRedraw() override;
    void resetNeedsRedraw() override;

    void registerApp(const char* category, OswAppV2* app);
    template<typename T>
    void registerAppLazy(const char* category) {
      if(!this->apps.count(category))
        this->apps.insert({category, {}});
      this->apps.at(category).push_back({nullptr});
      this->apps.at(category).back().set<T>();
    };
  private:
    class LazyInit {
      public:
        LazyInit(OswAppV2* given) {
          this->ptr = given;
        };

        template<typename T>
        void set() {
          this->init = []() -> OswAppV2* {
            return new T();
          };
        }

        OswAppV2* operator->() {
          if(this->ptr == nullptr)
            this->ptr = this->init();
          return this->ptr;
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
    std::map<const char*, std::list<LazyInit>> apps;
    LazyInit* current = nullptr;
    const char* defaultCategory;
    const size_t defaultAppIndex;
    bool showDrawer = false;
    size_t highlightCategoryIndex = 0;
    size_t highlightAppIndex = 0;
    size_t categoryIndexOffset = 0;
    LazyInit* highlightApp = nullptr;

    LazyInit& getCurrent();
};