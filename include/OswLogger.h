#pragma once

#include <mutex>
#include <memory>
#include <Arduino.h>
#include <OswSerial.h>
#include <stdarg.h>

#include <WString.h>
#include <string_view>

class OswLogger {
  public:
    OswLogger() {};
    ~OswLogger() {};

    static inline OswLogger* getInstance() {
        if (instance == nullptr)
            instance.reset(new OswLogger());
        return instance.get();
    };
    static void resetInstance() {
        instance.reset();
    };

    inline void error(const char* file, const unsigned int line, auto&& ... message) {
        this->log(file, line, severity_t::E, std::forward<decltype(message)>(message)...);
    };

    inline void warning(const char* file, const unsigned int line, auto&& ... message) {
        this->log(file, line, severity_t::W, std::forward<decltype(message)>(message)...);
    };

    inline void info(const char* file, const unsigned int line, auto&& ... message) {
        this->log(file, line, severity_t::I, std::forward<decltype(message)>(message)...);
    };

    inline void debug(const char* file, const unsigned int line, auto&& ... message) {
        this->log(file, line, severity_t::D, std::forward<decltype(message)>(message)...);
    };
  private:
    static std::unique_ptr<OswLogger> instance;
    std::mutex m_lock;
    enum class severity_t { D, I, W, E };

    // Used to process the variadic arguments in a type-safe (and aware) manner -> https://stackoverflow.com/a/15711171
    inline void do_in_order() {};
    template<typename Lambda0, typename... Lambdas>
    inline void do_in_order( Lambda0&& L0, Lambdas&& ... Ls ) {
        std::forward<Lambda0>(L0)();
        do_in_order( std::forward<Lambdas>(Ls)... );
    };

    void log(const char* file, const unsigned int line, const severity_t severity, auto&& ... message) {
        std::lock_guard<std::mutex> guard(this->m_lock);
        OswSerial* serial = OswSerial::getInstance();
        this->prefix(file, line, severity);

        do_in_order([&]() {
            // If performance would be no issue, we could just use String(...) and treat everything the same for the '\n'-iteration...
            if constexpr ( requires { std::string_view(message); }) { // If you can ignore char*-s, you could also use std::ranges::input_range<decltype(message)>
                for(auto& c : std::string_view(message)) {
                    if (c == '\n') {
                        serial->println();
                        this->prefix(file, line, severity);
                    } else
                        serial->putc(c);
                }
            } else {
                serial->print(message);
            }
        } ...);

        serial->println();
    };

    void prefix(const char* file, const unsigned int line, const severity_t severity) {
        OswSerial* serial = OswSerial::getInstance();

        switch(severity) {
        case severity_t::D:
            serial->print("D: ");
            break;
        case severity_t::I:
            serial->print("I: ");
            break;
        case severity_t::W:
            serial->print("W: ");
            break;
        case severity_t::E:
            serial->print("E: ");
            break;
        default:
            throw std::logic_error("Unknown severity level");
        }

#ifndef NDEBUG
        serial->print(file);
        serial->putc('@');
        serial->print(line);
        serial->print(": ");
#endif
    };
};

// Following defines are used to quickly log something - and to optimize code in case of debug-compiles
#ifndef NDEBUG
#define OSW_LOG_D(message...) OswLogger::getInstance()->debug(__FILE__, __LINE__, message)
#else
#define OSW_LOG_D(...)
#endif
#define OSW_LOG_I(message...) OswLogger::getInstance()->info(__FILE__, __LINE__, message)
#define OSW_LOG_W(message...) OswLogger::getInstance()->warning(__FILE__, __LINE__, message)
#define OSW_LOG_E(message...) OswLogger::getInstance()->error(__FILE__, __LINE__, message)

#ifdef OSW_EMULATOR
#define OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED OSW_LOG_W(__FUNCTION__, "() Not implemented!")
#endif
