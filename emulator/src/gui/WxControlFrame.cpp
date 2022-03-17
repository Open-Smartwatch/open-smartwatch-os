#include "../../include/Emulator.hpp"

#include "../../include/gui/WxControlFrame.hpp"

WxControlFrame::WxControlFrame(): wxFrame(nullptr, wxID_ANY, "OSW-OS Emulator Control") {
    // Stuff
    this->CreateStatusBar();

    // Menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    this->SetMenuBar(menuBar);
    this->Bind(wxEVT_MENU, &WxControlFrame::onAbout, this, wxID_ABOUT);
    this->Bind(wxEVT_MENU, &WxControlFrame::onExit, this, wxID_EXIT);

    wxPanel* rootPanel = new wxPanel(this, wxID_ANY);
    
    // Control pages
    wxNotebook* settingsNotebook = new wxNotebook(rootPanel, wxID_ANY);

    // Control: Input
    wxPanel* inputTabPanel = new wxPanel(settingsNotebook, wxID_ANY);
    wxBoxSizer* inputTabPanelSizer = new wxBoxSizer(wxVERTICAL);
    this->btn1 = new wxToggleButton(inputTabPanel, WxElementID::BUTTON1, "Button 1");
    this->btn2 = new wxToggleButton(inputTabPanel, WxElementID::BUTTON2, "Button 2");
    this->btn3 = new wxToggleButton(inputTabPanel, WxElementID::BUTTON3, "Button 3");
    inputTabPanelSizer->Add(this->btn1);
    inputTabPanelSizer->Add(this->btn2);
    inputTabPanelSizer->Add(this->btn3);
    this->btn1->Bind(wxEVT_LEFT_DOWN, &WxControlFrame::onInputButtonChange, this, -1, -1, this->btn1);
    this->btn1->Bind(wxEVT_LEFT_UP, &WxControlFrame::onInputButtonChange, this, -1, -1, this->btn1);
    this->btn2->Bind(wxEVT_LEFT_DOWN, &WxControlFrame::onInputButtonChange, this, -1, -1, this->btn2);
    this->btn2->Bind(wxEVT_LEFT_UP, &WxControlFrame::onInputButtonChange, this, -1, -1, this->btn2);
    this->btn3->Bind(wxEVT_LEFT_DOWN, &WxControlFrame::onInputButtonChange, this, -1, -1, this->btn3);
    this->btn3->Bind(wxEVT_LEFT_UP, &WxControlFrame::onInputButtonChange, this, -1, -1, this->btn3);
    inputTabPanel->SetSizer(inputTabPanelSizer);

    // Add notebook to root panel
    settingsNotebook->AddPage(inputTabPanel, "Input");
    wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
    panelSizer->Add(settingsNotebook, 1, wxEXPAND);
    rootPanel->SetSizer(panelSizer);
}

void WxControlFrame::onExit(wxCommandEvent& event) {
    Close(true);
}

void WxControlFrame::onAbout(wxCommandEvent& event) {
    wxMessageBox("This is the emulator for the open-smartwatch operating system.", "About OSW-OS Emulator", wxOK | wxICON_INFORMATION);
}

void WxControlFrame::onInputButtonChange(wxMouseEvent& event) {
    const bool pressed = event.ButtonDown();
    wxToggleButton* btn = static_cast<wxToggleButton*>(event.GetEventUserData());
    btn->SetValue(pressed);
    if(btn->GetId() == this->btn1->GetId())
        OswEmulator::instance->setButton(0, pressed);
    else if(btn->GetId() == this->btn2->GetId())
        OswEmulator::instance->setButton(1, pressed);
    else if(btn->GetId() == this->btn3->GetId())
        OswEmulator::instance->setButton(2, pressed);
    else
        throw std::runtime_error("Unknown button");
}