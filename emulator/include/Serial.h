#pragma once

#include <iostream>
#include <list>
#include <string>
#include <sstream>

#include "Defines.h"

class Serial_t {
  public:
    Serial_t() {};
    ~Serial_t() {};

    std::list<std::stringstream> buffer;

    void setBuffered(bool buffered) {
        this->isBuffered = buffered;
        this->addBufferNewline = true;
        if(!this->isBuffered)
            this->buffer.clear();
    }

    template<typename T>
    void print(T smth) {
        if(this->isBuffered) {
            if(this->addBufferNewline) {
                this->buffer.push_back({});
                this->addBufferNewline = false;
            }
            this->buffer.back() << smth;
        } else
            std::cout << smth << std::flush;
    }

    template<typename T>
    void println(T smth) {
        this->print(smth);
        this->println();
    }

    void println() {
        if(this->isBuffered)
            this->addBufferNewline = true;
        else
            std::cout << std::endl;
    }

    void begin(int b) {
        std::cout << "Serial initialized with " << b << " bauds." << std::endl;
    }
  private:
    bool isBuffered = false;
    bool addBufferNewline = true;
};

extern Serial_t Serial;