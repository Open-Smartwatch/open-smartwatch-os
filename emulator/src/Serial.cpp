#include "../include/Serial.h"
#include <OswLogger.h>

#include <iostream>
#include <unistd.h>
#include <fcntl.h>

Serial_t Serial;

Serial_t::Serial_t() {
    // put the stdin-stream into non-blocking mode to avoid hanging in case available() is called
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

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

void Serial_t::flush() {
    if(this->bauds < 1)
        return;
    if(!this->buffered)
        std::cout << std::flush;
}

int Serial_t::available() {
    char c;
    if(::read(STDIN_FILENO, &c, 1) > 0) {
        this->inputBuffer.push_back(c);
        return true;
    }
    return false;
}

int Serial_t::read() {
    if(this->inputBuffer.empty())
        return -1;
    char c = this->inputBuffer.front();
    this->inputBuffer.pop_front();
    return c;
}
