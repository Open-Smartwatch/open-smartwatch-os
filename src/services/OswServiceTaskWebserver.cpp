#include <WebServer.h>
#include <Update.h> // OTA by file upload
#include <HTTPClient.h> // OTA by uri

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
  String index = "<a href='/config'>Config</a><br><a href='/update'>Update</a><br><b>!!!WIP!!!</b>";
  this->m_webserver->send_P(200, "text/html", index.c_str(), index.length());
}

void OswServiceTaskWebserver::handleUpdate() {
  // TODO Rewrite using the css & js bundles already included!
  // TODO Do not use externally hosted jQuery... Brrrr...
  // TODO Add active update url input & request

  /**
   * When you ever need to test the ota by uri update method, just serve the
   * firmware.bin file using "python3 -m http.server" inside the respective directory.
   */
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
    // MD5 calc from here: https://stackoverflow.com/questions/14733374/how-to-generate-an-md5-file-hash-in-javascript/49050183
    "var MD5 = function(d){var r = M(V(Y(X(d),8*d.length)));return r.toLowerCase()};function M(d){for(var _,m=\"0123456789ABCDEF\",f=\"\",r=0;r<d.length;r++)_=d.charCodeAt(r),f+=m.charAt(_>>>4&15)+m.charAt(15&_);return f}function X(d){for(var _=Array(d.length>>2),m=0;m<_.length;m++)_[m]=0;for(m=0;m<8*d.length;m+=8)_[m>>5]|=(255&d.charCodeAt(m/8))<<m%32;return _}function V(d){for(var _=\"\",m=0;m<32*d.length;m+=8)_+=String.fromCharCode(d[m>>5]>>>m%32&255);return _}function Y(d,_){d[_>>5]|=128<<_%32,d[14+(_+64>>>9<<4)]=_;for(var m=1732584193,f=-271733879,r=-1732584194,i=271733878,n=0;n<d.length;n+=16){var h=m,t=f,g=r,e=i;f=md5_ii(f=md5_ii(f=md5_ii(f=md5_ii(f=md5_hh(f=md5_hh(f=md5_hh(f=md5_hh(f=md5_gg(f=md5_gg(f=md5_gg(f=md5_gg(f=md5_ff(f=md5_ff(f=md5_ff(f=md5_ff(f,r=md5_ff(r,i=md5_ff(i,m=md5_ff(m,f,r,i,d[n+0],7,-680876936),f,r,d[n+1],12,-389564586),m,f,d[n+2],17,606105819),i,m,d[n+3],22,-1044525330),r=md5_ff(r,i=md5_ff(i,m=md5_ff(m,f,r,i,d[n+4],7,-176418897),f,r,d[n+5],12,1200080426),m,f,d[n+6],17,-1473231341),i,m,d[n+7],22,-45705983),r=md5_ff(r,i=md5_ff(i,m=md5_ff(m,f,r,i,d[n+8],7,1770035416),f,r,d[n+9],12,-1958414417),m,f,d[n+10],17,-42063),i,m,d[n+11],22,-1990404162),r=md5_ff(r,i=md5_ff(i,m=md5_ff(m,f,r,i,d[n+12],7,1804603682),f,r,d[n+13],12,-40341101),m,f,d[n+14],17,-1502002290),i,m,d[n+15],22,1236535329),r=md5_gg(r,i=md5_gg(i,m=md5_gg(m,f,r,i,d[n+1],5,-165796510),f,r,d[n+6],9,-1069501632),m,f,d[n+11],14,643717713),i,m,d[n+0],20,-373897302),r=md5_gg(r,i=md5_gg(i,m=md5_gg(m,f,r,i,d[n+5],5,-701558691),f,r,d[n+10],9,38016083),m,f,d[n+15],14,-660478335),i,m,d[n+4],20,-405537848),r=md5_gg(r,i=md5_gg(i,m=md5_gg(m,f,r,i,d[n+9],5,568446438),f,r,d[n+14],9,-1019803690),m,f,d[n+3],14,-187363961),i,m,d[n+8],20,1163531501),r=md5_gg(r,i=md5_gg(i,m=md5_gg(m,f,r,i,d[n+13],5,-1444681467),f,r,d[n+2],9,-51403784),m,f,d[n+7],14,1735328473),i,m,d[n+12],20,-1926607734),r=md5_hh(r,i=md5_hh(i,m=md5_hh(m,f,r,i,d[n+5],4,-378558),f,r,d[n+8],11,-2022574463),m,f,d[n+11],16,1839030562),i,m,d[n+14],23,-35309556),r=md5_hh(r,i=md5_hh(i,m=md5_hh(m,f,r,i,d[n+1],4,-1530992060),f,r,d[n+4],11,1272893353),m,f,d[n+7],16,-155497632),i,m,d[n+10],23,-1094730640),r=md5_hh(r,i=md5_hh(i,m=md5_hh(m,f,r,i,d[n+13],4,681279174),f,r,d[n+0],11,-358537222),m,f,d[n+3],16,-722521979),i,m,d[n+6],23,76029189),r=md5_hh(r,i=md5_hh(i,m=md5_hh(m,f,r,i,d[n+9],4,-640364487),f,r,d[n+12],11,-421815835),m,f,d[n+15],16,530742520),i,m,d[n+2],23,-995338651),r=md5_ii(r,i=md5_ii(i,m=md5_ii(m,f,r,i,d[n+0],6,-198630844),f,r,d[n+7],10,1126891415),m,f,d[n+14],15,-1416354905),i,m,d[n+5],21,-57434055),r=md5_ii(r,i=md5_ii(i,m=md5_ii(m,f,r,i,d[n+12],6,1700485571),f,r,d[n+3],10,-1894986606),m,f,d[n+10],15,-1051523),i,m,d[n+1],21,-2054922799),r=md5_ii(r,i=md5_ii(i,m=md5_ii(m,f,r,i,d[n+8],6,1873313359),f,r,d[n+15],10,-30611744),m,f,d[n+6],15,-1560198380),i,m,d[n+13],21,1309151649),r=md5_ii(r,i=md5_ii(i,m=md5_ii(m,f,r,i,d[n+4],6,-145523070),f,r,d[n+11],10,-1120210379),m,f,d[n+2],15,718787259),i,m,d[n+9],21,-343485551),m=safe_add(m,h),f=safe_add(f,t),r=safe_add(r,g),i=safe_add(i,e)}return Array(m,f,r,i)}function md5_cmn(d,_,m,f,r,i){return safe_add(bit_rol(safe_add(safe_add(_,d),safe_add(f,i)),r),m)}function md5_ff(d,_,m,f,r,i,n){return md5_cmn(_&m|~_&f,d,_,r,i,n)}function md5_gg(d,_,m,f,r,i,n){return md5_cmn(_&f|m&~f,d,_,r,i,n)}function md5_hh(d,_,m,f,r,i,n){return md5_cmn(_^m^f,d,_,r,i,n)}function md5_ii(d,_,m,f,r,i,n){return md5_cmn(m^(_|~f),d,_,r,i,n)}function safe_add(d,_){var m=(65535&d)+(65535&_);return(d>>16)+(_>>16)+(m>>16)<<16|65535&m}function bit_rol(d,_){return d<<_|d>>>32-_};"
    "function sub(obj){"
    "var fileName = obj.value.split('\\\\');"
    "document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
    "};"
    "$('form').submit(function(e){"
    "e.preventDefault();"
    "var form = $('#upload_form')[0];"
    "var data = new FormData(form);"
    "$.ajax({"
    "url: '/ota/passive',"
    "headers: { 'x-UpdateHash': MD5($('#file')[0].files[0]) },"
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

void OswServiceTaskWebserver::handleActiveOTARequest() {
  // Check if config was received
  if (this->m_webserver->hasArg("plain")== false) {
    this->m_webserver->send(400, "text/plain", "URL missing.");
    return;
  }

  // Check if we've got an MD5;URL using http
  String updateMD5;
  String updateURL;
  {
    String temp = this->m_webserver->arg("plain");
    int pos = temp.indexOf(';');
    if(pos == 32) //Okay, we assume the first 32 chars are the md5 hex digset
      updateMD5 = temp.substring(0, pos);
    updateURL = temp.substring(pos + 1);
  }
  if(!updateURL.startsWith("http://")) {
    this->m_webserver->send(400, "text/plain", "URI has wrong protocol, only HTTP is allowed.");
    return;
  }
  Serial.println(String(__FILE__) + ": [OTA] URL: " + updateURL);

  //Perform the update
  HTTPClient http;
  http.begin(updateURL);
  http.useHTTP10(true); //To prevent any encodings

  int code = http.GET();
  int size = http.getSize();

  if(code != 200 or size == 0) {
    Serial.println(String(__FILE__) + ": [OTA] Fetch failed: " + http.errorToString(code));
    this->m_webserver->send(400, "text/plain", http.errorToString(code));
    return;
  }

  if(!Update.begin(size)) {
    Serial.println(String(__FILE__) + ": [OTA] Begin failed: " + Update.errorString());
    this->m_webserver->send(400, "text/plain", Update.errorString());
    return;
  }

  if(updateMD5.length()) {
    Update.setMD5(updateMD5.c_str());
    Serial.println(String(__FILE__) + ": [OTA] MD5: " + updateMD5);
  }

  if(Update.writeStream(*http.getStreamPtr()) != size) {
    Serial.println(String(__FILE__) + ": [OTA] Write failed: " + Update.errorString());
    this->m_webserver->send(400, "text/plain", Update.errorString());
    return;
  }

  if(!Update.end()) {
    Serial.println(String(__FILE__) + ": [OTA] Finish failed: " + Update.errorString());
    this->m_webserver->send(400, "text/plain", Update.errorString());
    return;
  }

  Serial.println(String(__FILE__) + ": [OTA] Finished!");
  this->m_webserver->send(200, "text/plain", "OK");
  this->m_restartRequest = true;
}

void OswServiceTaskWebserver::handlePassiveOTARequest() {
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
      String updateMD5 = this->m_webserver->header("x-UpdateHash");
      if(updateMD5.length()) {
        Update.setMD5(updateMD5.c_str());
        Serial.println(String(__FILE__) + ": [OTA] MD5: " + updateMD5);
      }
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
  this->m_webserver->on("/ota/passive", HTTP_POST, [this] { this->handleAuthenticated([this] { this->handlePassiveOTARequest(); }); }, [this] { this->handleAuthenticated([this] { this->handleOTAFile(); }); });
  this->m_webserver->on("/ota/active", [this] { this->handleAuthenticated([this] { this->handleActiveOTARequest(); }); });
  this->m_webserver->on("/config", [this] { this->handleAuthenticated([this] { this->handleConfig(); }); });
  this->m_webserver->on("/bundle.min.css", [this] { this->handleUnauthenticated([this] { this->handleCss(); }); });
  this->m_webserver->on("/bundle.min.js", [this] { this->handleUnauthenticated([this] { this->handleJs(); }); });
  this->m_webserver->on("/config.json", [this] { this->handleAuthenticated([this] { this->handleConfigJson(); }); });
  this->m_webserver->on("/data.json", HTTP_POST, [this] { this->handleAuthenticated([this] { this->handleDataJson(); }); });
  this->m_webserver->begin();

  /**
   * We also have to make sure to collect the following headers on any request.
   * We are using headers, as post keys could be processed after the file upload,
   * therefore we would receive the md5 AFTER the update is already complete.
   */
  const char* headers[] = { "x-UpdateHash" };
  this->m_webserver->collectHeaders(headers, 1);
  
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