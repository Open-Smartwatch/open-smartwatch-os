#include "../include/Preferences.h"
#include <OswLogger.h>

std::string Preferences::preferencesFolderName = "emulator_nvs";

bool Preferences::begin(const char* name, bool readOnly) {
    this->readOnly = readOnly;
    // Init "NVS"
    nvs_flash_init();
    this->name = std::string(name);
    // Init Jzon tree by loading (existing) file
    this->path = std::filesystem::path(preferencesFolderName) / (this->name + ".json");
    if(std::filesystem::exists(this->path)) {
        Jzon::Parser p;
        this->node = p.parseFile(this->path.string());
    } else {
        if(this->readOnly)
            return false; // In case we are read-only we can't init the path!
        this->node = Jzon::object();
    }
    return true;
}

size_t Preferences::putBytes(const char* key, const void* value, size_t len) {
    // As we can't store binary data into Jzon, we will instead write a file with an appropriate name...
    if(this->readOnly)
        return 0; // Not in read-only mode
    std::ofstream outFile(this->getBytesPath(key), std::ios::binary);
    outFile.write((const char*) value, len);
    outFile.close();
    return len;
}

size_t Preferences::getBytes(const char* key, void* buf, size_t maxLen) {
    std::ifstream inFile(this->getBytesPath(key), std::ios::binary);
    inFile.read((char*) buf, maxLen);
    size_t readBytes = inFile.gcount();
    inFile.close();
    return readBytes;
}

void Preferences::serialize() {
    if(this->readOnly)
        return; // Not in read-only mode
    Jzon::Writer w;
    w.writeFile(this->node, path.string());
    OSW_LOG_D("Written preferences of namespace ", this->name);
}

bool Preferences::clear() {
    if(this->readOnly)
        return false; // Not in read-only mode
    this->node = Jzon::object();
    this->serialize();
    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::path(preferencesFolderName)))
        if(entry.path().filename().generic_string().starts_with(this->name + "_"))
            std::filesystem::remove(entry.path());
    return true;
}

bool Preferences::remove(const char* key) {
    if(this->readOnly)
        return false; // Not in read-only mode
    if(std::filesystem::remove(this->getBytesPath(key)))
        return true;
    if(this->node.has(key)) {
        this->node.remove(key);
        this->serialize();
        return true;
    }
    return false;
}