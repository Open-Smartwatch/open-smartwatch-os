#include "../include/Serial.h"
#include <OswLogger.h>

Serial_t Serial;

void Serial_t::setBuffered(bool buffered) {
    this->buffered = buffered;
    this->addBufferNewline = true;
    if(!this->buffered)
        this->buffer.clear();
}

bool Serial_t::isBuffered() const {
    return this->buffered;
}

void Serial_t::begin(int b) {
    this->bauds = b;
    OSW_LOG_D("Serial initialized with ", b, " bauds.");
}

void Serial_t::println() {
    if(this->bauds < 1)
        return;
    if(this->buffered)
        this->addBufferNewline = true;
    else
        std::cout << std::endl;
}