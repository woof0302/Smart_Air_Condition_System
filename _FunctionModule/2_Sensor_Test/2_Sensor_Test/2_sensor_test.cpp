#include <iostream>
#include <thread>
#include <chrono>
#include <gpiod.h>

#include "SimpleDHT.h"

#define PHOTO_GPIO 21
#define DHT_GPIO   26

int main()
{
    SimpleDHT11 dht11(DHT_GPIO);

    gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");

    if (!chip)
    {
        std::cerr << "gpiochip open failed\n";
        return 1;
    }

    gpiod_line* photoLine =
        gpiod_chip_get_line(chip, PHOTO_GPIO);

    if (!photoLine)
    {
        std::cerr << "GPIO21 open failed\n";
        return 1;
    }

    gpiod_line_request_input(photoLine, "photo_sensor");

    int previousPhoto =
        gpiod_line_get_value(photoLine);

    while (true)
    {
        // DHT11 Temp,Hum Sensor

        byte temperature = 0;
        byte humidity = 0;

        int err =
            dht11.read(&temperature,
                &humidity,
                nullptr);

        std::cout << "\n========================\n";

        if (err == SimpleDHTErrSuccess)
        {
            std::cout
                << "Temperature : "
                << static_cast<int>(temperature)
                << " °C\n";

            std::cout
                << "Humidity : "
                << static_cast<int>(humidity)
                << " %\n";
        }
        else
        {
            std::cout
                << "DHT11 읽기 실패 ("
                << err
                << ")\n";
        }
       
        // ES92B4   Photon Sensor
        int photoValue =
            gpiod_line_get_value(photoLine);

        std::cout
            << "Photon : "
            << photoValue
            << '\n';

        if (photoValue != previousPhoto)
        {
            if (photoValue == 1)
            {
                std::cout
                    << ">>> Passed!\n";
            }

            previousPhoto = photoValue;
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(2));
    }

    gpiod_line_release(photoLine);
    gpiod_chip_close(chip);

    return 0;
}