#include "../include/Preferences.h"

const char* Preferences::preferencesFolderName = "emulator_nvs";

bool Preferences::begin(const char* name, bool) {
    // Init "NVS"
    nvs_flash_init();
    this->name = std::string(name);
    // Init Jzon tree by loading (existing) file
    this->path = std::filesystem::path(preferencesFolderName) / (this->name + ".json");
    if(std::filesystem::exists(this->path)) {
        Jzon::Parser p;
        this->node = p.parseFile(this->path.string());
    } else {
        this->node = Jzon::object();
    }
    return true;
}

size_t Preferences::putBytes(const char* key, const void* value, size_t len) {
    // As we can't store binary data into Jzon, we will instead write a file with an appropriate name...
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
    Jzon::Writer w;
    w.writeFile(this->node, path.string());
    std::cout << "Written preferences of namespace " << this->name << std::endl;
}