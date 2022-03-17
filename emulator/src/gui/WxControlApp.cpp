#include "../../include/gui/WxControlApp.hpp"

bool WxControlApp::OnInit() {
    wxInitAllImageHandlers();
    this->frame = new WxControlFrame();
    this->frame->Show();
    return true;
}