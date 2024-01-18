#include "GlobalConfig.h"

void GPIOInit()
{
    // pinMode(ST_PIN, INPUT);

    // pinMode(PWR_PIN, OUTPUT);
    // digitalWrite(PWR_PIN, LOW);

    pinMode(ESP_PIN, OUTPUT);
    digitalWrite(ESP_PIN, LOW);

    pinMode(LUN_PIN, OUTPUT);
    digitalWrite(LUN_PIN, LOW);

    pinMode(CAP_PIN, OUTPUT);
    digitalWrite(CAP_PIN, LOW);
}
