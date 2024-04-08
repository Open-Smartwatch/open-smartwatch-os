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

    void setBuffered(bool buffered);
    bool isBuffered() const;
    void begin(int b);

    template<typename T>
    void print(T smth) {
        if(this->bauds < 1)
            return;
        if(this->buffered) {
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
        if(this->bauds < 1)
            return;
        this->print(smth);
        this->println();
    }

    void println();

    void flush() {}
  private:
    int bauds = 0;
    bool buffered = false;
    bool addBufferNewline = true;
};

extern Serial_t Serial;
