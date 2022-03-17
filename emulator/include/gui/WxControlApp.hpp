#pragma once

#include <wx/wx.h>

#include "WxControlFrame.hpp"

class WxControlApp: public wxApp {
  public:
    bool OnInit();
  private:
    WxControlFrame* frame = nullptr;
};