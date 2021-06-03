#include <WebServer.h>
#include <Update.h>

#include <assets/bundle.min.css.gz.h>
#include <assets/bundle.min.js.gz.h>
#include <assets/index.html.gz.h>

#include "osw_hal.h"
#include <osw_ui.h>
#include "services/OswServiceTasks.h"
#include "services/OswServiceTaskWiFi.h"
#include "services/OswServiceManager.h"
#include "./services/OswServiceTaskWebserver.h"

void OswServiceTaskWebserver::handleAuthenticated(std::function<void(void)> handler) {
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": " + this->m_webserver->uri());
#endif
  if (!this->m_webserver->authenticate("admin", this->m_uiPassword.c_str())) {
    return this->m_webserver->requestAuthentication();
  }
  handler();
}

void OswServiceTaskWebserver::handleUnauthenticated(std::function<void(void)> handler) {
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": " + this->m_webserver->uri());
#endif
  handler();
}

void OswServiceTaskWebserver::handleIndex() {
  // TODO Write pretty index
  // TODO Move system info here
  String index = "<a href='/config'>Config</a><a href='/update'>Update</a><b>!!!WIP!!!</b>";
  this->m_webserver->send_P(200, "text/html", index.c_str(), index.length());
}

void OswServiceTaskWebserver::handleUpdate() {
  // TODO Rewrite using the css & js bundles already included!
  // TODO Do not use externally hosted jQuery... Brrrr...
  String style =
    "<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
    "input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
    "#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
    "#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
    "form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
    ".btn{background:#3498db;color:#fff;cursor:pointer}</style>";
  String serverIndex =
    "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
    "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
    "<label id='file-input' for='file'>   Choose file...</label>"
    "<input type='submit' class=btn value='Update'>"
    "<br><br>"
    "<div id='prg'></div>"
    "<br><div id='prgbar'><div id='bar'></div></div><br></form>"
    "<script>"
    "function sub(obj){"
    "var fileName = obj.value.split('\\\\');"
    "document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
    "};"
    "$('form').submit(function(e){"
    "e.preventDefault();"
    "var form = $('#upload_form')[0];"
    "var data = new FormData(form);"
    "$.ajax({"
    "url: '/ota',"
    "type: 'POST',"
    "data: data,"
    "contentType: false,"
    "processData:false,"
    "xhr: function() {"
    "var xhr = new window.XMLHttpRequest();"
    "xhr.upload.addEventListener('progress', function(evt) {"
    "if (evt.lengthComputable) {"
    "var per = evt.loaded / evt.total;"
    "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
    "$('#bar').css('width',Math.round(per*100) + '%');"
    "}"
    "}, false);"
    "return xhr;"
    "},"
    "success:function(d, s) {"
    "console.log('success!') "
    "},"
    "error: function (a, b, c) {"
    "}"
    "});"
    "});"
    "</script>" + style;
  this->m_webserver->send_P(200, "text/html", serverIndex.c_str(), serverIndex.length());
}

void OswServiceTaskWebserver::handleOTARequest() {
  if(!Update.hasError()) {
    this->m_webserver->send(200, "text/plain", "OK");
    this->m_restartRequest = true;
  } else {
    this->m_webserver->send(400, "text/plain", Update.errorString());
  }
}

void OswServiceTaskWebserver::handleOTAFile() {
  try {
    HTTPUpload& upload = this->m_webserver->upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.println(String(__FILE__) + ": [OTA] Name: " + upload.filename);
      if (!Update.begin(UPDATE_SIZE_UNKNOWN))
        throw false;
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      // This is maybe not the best indicator for a defective update, but it works well enough...
      if(upload.currentSize == 0)
        throw true;
      Serial.println(String(__FILE__) + ": [OTA] Next chunk: " + upload.currentSize);
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        throw true;
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true))
        Serial.println(String(__FILE__) + ": [OTA] Finished after " + upload.totalSize + " bytes!");
      else
        throw false;
    }
  } catch (bool abortUpdate) {
    if(abortUpdate)
      Update.abort();
    Serial.println(String(__FILE__) + ": [OTA] Failed: " + Update.errorString());
    return;
  }
}

void OswServiceTaskWebserver::handleConfig() {
  this->m_webserver->sendHeader(F("Content-Encoding"), F("gzip"));
  this->m_webserver->send_P(200, "text/html", (const char*)dist_data_index_html_gz, dist_data_index_html_gz_len);
}

void OswServiceTaskWebserver::handleCss() {
  this->m_webserver->sendHeader(F("Content-Encoding"), F("gzip"));
  this->m_webserver->send_P(200, "text/css", (const char*)dist_data_bundle_min_css_gz, dist_data_bundle_min_css_gz_len);
}

void OswServiceTaskWebserver::handleJs() {
  this->m_webserver->sendHeader(F("Content-Encoding"), F("gzip"));
  this->m_webserver->send_P(200, "application/javascript", (const char*)dist_data_bundle_min_js_gz,
                 dist_data_bundle_min_js_gz_len);
}

void OswServiceTaskWebserver::handleConfigJson() { this->m_webserver->send(200, "application/json", OswConfig::getInstance()->getConfigJSON()); }

void OswServiceTaskWebserver::handleDataJson() {
  if (this->m_webserver->hasArg("plain") == false) {
    this->m_webserver->send(422, "application/json", "{\"error\": \"CFG_MISSING\"}");
    return;
  }

  OswConfig::getInstance()->enableWrite();
  OswConfig::getInstance()->parseDataJSON(this->m_webserver->arg("plain").c_str());
  OswConfig::getInstance()->disableWrite();

  // TODO: error handling?
  this->m_webserver->send(200, "application/json", "{\"success\":true}");
  OswUI::getInstance()->resetTextColors();
}

void OswServiceTaskWebserver::setup(OswHal* hal) {
  OswServiceTask::setup(hal);
  //Do not call enableWebserver() here, so the loop() / developers could do this later on as needed
}

void OswServiceTaskWebserver::loop(OswHal* hal) {
  if(OswServiceAllTasks::wifi.isConnected())
    this->enableWebserver();
  else
    this->disableWebserver();
  if (this->m_webserver) this->m_webserver->handleClient();
  if(this->m_restartRequest) {
#ifdef DEBUG
    Serial.println(String(__FILE__) + ": REBOOT REQUEST RECEIVED. REBOOT IN 2 SECONDS!");
#endif
    sleep(2); // Just to make sure all web requests are finished...
    ESP.restart();
  }
}

void OswServiceTaskWebserver::stop(OswHal* hal) {
  OswServiceTask::stop(hal);
  this->disableWebserver(); //Make sure the webserver is also stopped
}

void OswServiceTaskWebserver::enableWebserver() {
  if(this->m_webserver)
    return;

  this->m_uiPassword = String((int)(rand() % 90000 + 10000));  // Generate a random ui password on loading

  this->m_webserver = new WebServer(80);
  this->m_webserver->on("/", [this] { this->handleAuthenticated([this] { this->handleIndex(); }); });
  this->m_webserver->on("/update", [this] { this->handleAuthenticated([this] { this->handleUpdate(); }); });
  this->m_webserver->on("/ota", HTTP_POST, [this] { this->handleAuthenticated([this] { this->handleOTARequest(); }); }, [this] { this->handleAuthenticated([this] { this->handleOTAFile(); }); });
  this->m_webserver->on("/config", [this] { this->handleAuthenticated([this] { this->handleConfig(); }); });
  this->m_webserver->on("/bundle.min.css", [this] { this->handleUnauthenticated([this] { this->handleCss(); }); });
  this->m_webserver->on("/bundle.min.js", [this] { this->handleUnauthenticated([this] { this->handleJs(); }); });
  this->m_webserver->on("/config.json", [this] { this->handleAuthenticated([this] { this->handleConfigJson(); }); });
  this->m_webserver->on("/data.json", HTTP_POST, [this] { this->handleAuthenticated([this] { this->handleDataJson(); }); });
  this->m_webserver->begin();
  
#ifdef DEBUG
    Serial.println(String(__FILE__) + ": Active.");
#endif
}

void OswServiceTaskWebserver::disableWebserver() {
  if (!this->m_webserver)
    return;

  this->m_webserver->stop();
  delete this->m_webserver;
  this->m_webserver = nullptr;

#ifdef DEBUG
    Serial.println(String(__FILE__) + ": Inactive.");
#endif
}

String OswServiceTaskWebserver::getPassword() const {
  return this->m_uiPassword;
}

WebServer* OswServiceTaskWebserver::getTaskWebserver() const {
  return this->m_webserver;
}