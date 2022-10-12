#pragma once

#include <mutex>
#include <Arduino.h>
#include <HardwareSerial.h> // For Serial.print(ln)
#include <stdarg.h>

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
    }

    template<typename... T>
    void log(const char* file, const unsigned int line, const severity_t severity, T&&... message) {
        std::lock_guard<std::mutex> guard(this->m_lock);
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
        Serial.print(file);
        Serial.print(":");
        Serial.print(line);
        Serial.print(" - ");

        do_in_order( [&](){
            Serial.print(std::forward<T>(message));
        }...);
    
        Serial.println();
    }
};

// Following defines are used to quickly log something - and to optimize code in case of debug-compiles
#define OSW_LOG_D(message...) OswLogger::getInstance()->debug(__FILE__, __LINE__, message)
#define OSW_LOG_I(message...) OswLogger::getInstance()->info(__FILE__, __LINE__, message)
#define OSW_LOG_W(message...) OswLogger::getInstance()->warning(__FILE__, __LINE__, message)
#define OSW_LOG_E(message...) OswLogger::getInstance()->error(__FILE__, __LINE__, message)