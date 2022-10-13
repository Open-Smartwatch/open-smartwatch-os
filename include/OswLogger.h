#pragma once

#include <mutex>
#include <Arduino.h>
#include <HardwareSerial.h> // For Serial.print(ln)
#include <stdarg.h>

#include <WString.h>
#include <string_view>

class OswLogger {
  public:
    OswLogger() {};
    ~OswLogger() {};

    static inline OswLogger* getInstance() {
        if (instance == nullptr)
            instance = new OswLogger();
        return instance;
    };

    template<typename... T>
    inline void error(const char* file, const unsigned int line, T&&... message) {
        this->log(file, line, severity_t::E, std::forward<T>(message)...);
    };

    template<typename... T>
    inline void warning(const char* file, const unsigned int line, T&&... message) {
        this->log(file, line, severity_t::W, std::forward<T>(message)...);
    };

    template<typename... T>
    inline void info(const char* file, const unsigned int line, T&&... message) {
        this->log(file, line, severity_t::I, std::forward<T>(message)...);
    };

    template<typename... T>
    inline void debug(const char* file, const unsigned int line, T&&... message) {
        this->log(file, line, severity_t::D, std::forward<T>(message)...);
    };
  private:
    static OswLogger* instance;
    std::mutex m_lock;
    enum class severity_t { D, I, W, E };

    // Used to process the variadic arguments in a type-safe (and aware) manner -> https://stackoverflow.com/a/15711171
    inline void do_in_order() {};
    template<typename Lambda0, typename... Lambdas>
    inline void do_in_order( Lambda0&& L0, Lambdas&&... Ls ) {
        std::forward<Lambda0>(L0)();
        do_in_order( std::forward<Lambdas>(Ls)... );
    };

    template<typename... T>
    void log(const char* file, const unsigned int line, const severity_t severity, T&&... message) {
        std::lock_guard<std::mutex> guard(this->m_lock);
        this->prefix(file, line, severity);

        do_in_order([&]() {
            // If performance is no issue, we could just use String(...) and treat everything the same for the '\n'-iteration...
            if constexpr (std::is_same<T, String>::value or std::is_same<T, std::string>::value or
                std::is_same<T, String&>::value or std::is_same<T, std::string&>::value) {
                // Iterate over message to find '\n', which trigger new lines...
                for(auto& c : message) {
                    if (c == '\n') {
                        Serial.println();
                        this->prefix(file, line, severity);
                    } else
                        Serial.print(c);
                }
            } else if constexpr (std::is_same<T, const char*>::value or std::is_same<T, char*>::value) {
                // Iterate over message to find '\n', which trigger new lines...
                for(auto& c : std::string_view(message)) {
                    if (c == '\n') {
                        Serial.println();
                        this->prefix(file, line, severity);
                    } else
                        Serial.print(c);
                }
            } else
                Serial.print(message);
        }...);
    
        Serial.println();
    };

    void prefix(const char* file, const unsigned int line, const severity_t severity) {
        if(severity == severity_t::D)
            Serial.print("D: ");
        else if(severity == severity_t::I)
            Serial.print("I: ");
        else if(severity == severity_t::W)
            Serial.print("W: ");
        else if(severity == severity_t::E)
            Serial.print("E: ");
        else
            throw std::logic_error("Unknown severity level");

#ifndef NDEBUG
        Serial.print(file);
        Serial.print("@");
        Serial.print(line);
        Serial.print(": ");
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
#define OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED OSW_LOG_W(__FUNCTION__, "() Not implemented!");
#endif