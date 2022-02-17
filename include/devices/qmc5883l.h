#pragma once

#include <QMC5883LCompass.h>

#include <devices/OswDevice.h>
#include <devices/interfaces/OswMagnetometerProvider.h>

namespace OswDevices {
    class QMC5883L : public OswMagnetometerProvider {
    public:
        QMC5883L() : OswMagnetometerProvider() {};
        virtual ~QMC5883L() {};

        virtual void setup() override;
        virtual void update() override;
        virtual void reset() override {};
        virtual void stop() override;

        virtual inline const char* getName() override { return "QMC5883L"; };

        virtual int getMagnetometerAzimuth() override;
        virtual inline unsigned char getMagnetometerProviderPriority() override { return 100; }; // This is a specialized device!
        int getMagnetometerX();
        int getMagnetometerY();
        int getMagnetometerZ();
        byte getMagnetometerBearing();
        void setMagnetometerCalibration(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max);
    private:
        QMC5883LCompass qmc5883l;
    };
};