#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/tglbtn.h>

class WxControlFrame: public wxFrame {
  public:
    WxControlFrame();
  private:
    enum WxElementID: int {
        BUTTON1,
        BUTTON2,
        BUTTON3
    };
    wxToggleButton* btn1 = nullptr;
    wxToggleButton* btn2 = nullptr;
    wxToggleButton* btn3 = nullptr;
    void onExit(wxCommandEvent& event);
    void onAbout(wxCommandEvent& event);
    void onInputButtonChange(wxMouseEvent& event);
};