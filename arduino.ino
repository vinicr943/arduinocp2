#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>
#include <EEPROM.h>
#include "DHT.h"

#define LOG_OPTION 1
#define SERIAL_OPTION 0
#define UTC_OFFSET -3

// Sensor DHT22
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Botão
#define BUTTON_PIN 5
#define DEBOUNCE_DELAY 200  // ms

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;

// Configurações da EEPROM
const int maxRecords = 100;
const int recordSize = 8;
int startAddress = 0;
int endAddress = maxRecords * recordSize;
int currentAddress = 0;

int lastLoggedMinute = -1;

// Triggers
float trigger_t_min = 20.0;
float trigger_t_max = 30.0;
float trigger_u_min = 30.0;
float trigger_u_max = 60.0;

// Controle das telas
int currentScreen = 0;

// Debounce não-bloqueante
bool lastButtonState = HIGH;
bool currentButtonState = HIGH;
unsigned long lastDebounceTime = 0;

// Status da temperatura
String statusTemperatura(float temp) {
    if (temp < trigger_t_min) return "BAIXA";
    else if (temp > trigger_t_max) return "ALTA";
    else return "BOA";
}

// Status da umidade
String statusUmidade(float hum) {
    if (hum < trigger_u_min) return "BAIXA";
    else if (hum > trigger_u_max) return "ALTA";
    else return "BOA";
}

// Vai para próxima tela
void nextScreen() {
    currentScreen++;
    if (currentScreen > 2) currentScreen = 0;
}

// Leitura do botão com debounce não-bloqueante
void checkButton() {
    bool reading = digitalRead(BUTTON_PIN);

    // Detecta borda de descida (HIGH → LOW = pressionado)
    if (reading == LOW && lastButtonState == HIGH) {
        unsigned long now = millis();
        if (now - lastDebounceTime > DEBOUNCE_DELAY) {
            lastDebounceTime = now;
            nextScreen();
        }
    }

    lastButtonState = reading;
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    dht.begin();
    Serial.begin(9600);

    lcd.init();
    lcd.backlight();

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Next Level");
    lcd.setCursor(1, 1);
    lcd.print("Inicializando");

    delay(2500);

    RTC.begin();
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));

    EEPROM.begin();
}

void loop() {
    DateTime now = RTC.now();

    int offsetSeconds = UTC_OFFSET * 3600;
    now = now.unixtime() + offsetSeconds;
    DateTime adjustedTime = DateTime(now);

    if (LOG_OPTION) {
        get_log();
    }

    float humidity    = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Registro por minuto
    if (adjustedTime.minute() != lastLoggedMinute) {
        lastLoggedMinute = adjustedTime.minute();

        digitalWrite(LED_BUILTIN, HIGH);
        delay(300);
        digitalWrite(LED_BUILTIN, LOW);

        if (temperature < trigger_t_min ||
            temperature > trigger_t_max ||
            humidity    < trigger_u_min ||
            humidity    > trigger_u_max) {

            int tempInt = (int)(temperature * 100);
            int humiInt = (int)(humidity * 100);

            EEPROM.put(currentAddress,     now.unixtime());
            EEPROM.put(currentAddress + 4, tempInt);
            EEPROM.put(currentAddress + 6, humiInt);

            getNextAddress();
        }
    }

    if (SERIAL_OPTION) {
        Serial.print("Temp: ");
        Serial.print(temperature);
        Serial.print(" C | ");
        Serial.print(statusTemperatura(temperature));
        Serial.print(" | Umid: ");
        Serial.print(humidity);
        Serial.print("% | ");
        Serial.println(statusUmidade(humidity));
    }

    // Verifica botão
    checkButton();

    // Atualiza LCD
    lcd.clear();

    // TELA 1 - Temperatura
    if (currentScreen == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Temperatura");
        lcd.setCursor(0, 1);
        lcd.print(temperature, 1);
        lcd.print((char)223);
        lcd.print("C ");
        lcd.print(statusTemperatura(temperature));
    }

    // TELA 2 - Umidade
    else if (currentScreen == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Umidade");
        lcd.setCursor(0, 1);
        lcd.print(humidity, 1);
        lcd.print("% ");
        lcd.print(statusUmidade(humidity));
    }

    // TELA 3 - Data e Hora
    else if (currentScreen == 2) {
        lcd.setCursor(0, 0);
        lcd.print(adjustedTime.day()   < 10 ? "0" : ""); lcd.print(adjustedTime.day());
        lcd.print("/");
        lcd.print(adjustedTime.month() < 10 ? "0" : ""); lcd.print(adjustedTime.month());
        lcd.print("/");
        lcd.print(adjustedTime.year());

        lcd.setCursor(0, 1);
        lcd.print(adjustedTime.hour()   < 10 ? "0" : ""); lcd.print(adjustedTime.hour());
        lcd.print(":");
        lcd.print(adjustedTime.minute() < 10 ? "0" : ""); lcd.print(adjustedTime.minute());
        lcd.print(":");
        lcd.print(adjustedTime.second() < 10 ? "0" : ""); lcd.print(adjustedTime.second());
    }

    delay(100);
}

void getNextAddress() {
    currentAddress += recordSize;
    if (currentAddress >= endAddress) currentAddress = 0;
}

void get_log() {
    Serial.println("Data stored in EEPROM:");
    Serial.println("Timestamp\t\tTemperature\tHumidity");

    for (int address = startAddress; address < endAddress; address += recordSize) {
        long timeStamp;
        int  tempInt;
        int  humiInt;

        EEPROM.get(address,     timeStamp);
        EEPROM.get(address + 4, tempInt);
        EEPROM.get(address + 6, humiInt);

        float temperature = tempInt / 100.0;
        float humidity    = humiInt / 100.0;

        if (timeStamp != 0xFFFFFFFF) {
            DateTime dt = DateTime(timeStamp);
            Serial.print(dt.timestamp(DateTime::TIMESTAMP_FULL));
            Serial.print("\t");
            Serial.print(temperature);
            Serial.print(" C\t\t");
            Serial.print(humidity);
            Serial.println(" %");
        }
    }
}
