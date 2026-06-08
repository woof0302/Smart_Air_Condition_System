#include <pigpio.h>
#include <iostream>
#include <thread>
#include <chrono>

constexpr int DHT_PIN = 26;
constexpr int ES92B4_PIN = 21;

struct DHT11Data
{
    float temperature;
    float humidity;
};

bool readDHT11(DHT11Data& data)
{
    uint8_t bits[5] = { 0 };

    gpioSetMode(DHT_PIN, PI_OUTPUT);
    gpioWrite(DHT_PIN, PI_LOW);
    gpioDelay(18000);

    gpioWrite(DHT_PIN, PI_HIGH);
    gpioDelay(40);

    gpioSetMode(DHT_PIN, PI_INPUT);

    uint32_t timeout = gpioTick();

    while (gpioRead(DHT_PIN) == PI_HIGH)
        if (gpioTick() - timeout > 100) return false;

    timeout = gpioTick();
    while (gpioRead(DHT_PIN) == PI_LOW)
        if (gpioTick() - timeout > 100) return false;

    timeout = gpioTick();
    while (gpioRead(DHT_PIN) == PI_HIGH)
        if (gpioTick() - timeout > 100) return false;

    for (int i = 0; i < 40; i++)
    {
        timeout = gpioTick();
        while (gpioRead(DHT_PIN) == PI_LOW)
            if (gpioTick() - timeout > 100) return false;

        uint32_t start = gpioTick();

        timeout = gpioTick();
        while (gpioRead(DHT_PIN) == PI_HIGH)
        {
            if (gpioTick() - timeout > 120)
                break;
        }

        uint32_t pulseLength = gpioTick() - start;

        bits[i / 8] <<= 1;
        if (pulseLength > 50)
            bits[i / 8] |= 1;
    }

    uint8_t checksum =
        (bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF;

    if (checksum != bits[4])
        return false;

    data.humidity = bits[0];
    data.temperature = bits[2];

    return true;
}

int main()
{
    if (gpioInitialise() < 0)
    {
        std::cerr << "pigpio init failed\n";
        return 1;
    }

    gpioSetMode(ES92B4_PIN, PI_INPUT);
    gpioSetPullUpDown(ES92B4_PIN, PI_PUD_UP);

    while (true)
    {
        DHT11Data dht;

        bool ok = readDHT11(dht);

        double esValue =
            gpioRead(ES92B4_PIN) ? 1.0 : 0.0;

        if (ok)
        {
            std::cout
                << "Temp: " << dht.temperature << " C, "
                << "Humidity: " << dht.humidity << " %, "
                << "ES92B4: " << esValue
                << std::endl;
        }
        else
        {
            std::cout
                << "DHT11 read failed, "
                << "ES92B4: " << esValue
                << std::endl;
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(2));
    }

    gpioTerminate();
    return 0;
}