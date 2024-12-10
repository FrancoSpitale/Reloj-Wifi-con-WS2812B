#include <NTPClient.h>
#include <FastLED.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>

// Configuración de FastLED
#define NUM_LEDS 256 // 4 matrices de 8x8 = 64 LEDs por matriz * 4 = 256 LEDs
#define DATA_PIN 4   // Pin D2 en el ESP8266

CRGB leds[NUM_LEDS];

// Configuración de NTPClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // UTC-3, actualiza cada 60 segundos

WiFiManager wifiManager;

unsigned long previousMillis = 0;       // Para manejar el intervalo
const long interval = 1000;             // Intervalo de 1 segundo
unsigned long lastUpdateTime = 0;       // Última hora sincronizada desde el servidor NTP (en segundos)
unsigned long localSeconds = 0;         // Tiempo local en segundos basado en millis()
bool ntpSync = false;                   // Indica si se ha sincronizado con NTP

void setup() {
  Serial.begin(115200);

  // Configurar WiFi con WiFiManager
  wifiManager.setConfigPortalTimeout(60); // Tiempo de espera del portal de configuración en segundos
  if (!wifiManager.autoConnect("RelojAP", "password")) {
    Serial.println("No se pudo conectar a WiFi. Entrando en modo de configuración.");
    wifiManager.startConfigPortal("RelojAP", "password");
  }

  // Inicializar NTPClient
  timeClient.begin();
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    lastUpdateTime = timeClient.getEpochTime();
    localSeconds = lastUpdateTime;
    ntpSync = true;
  }

  // Configurar FastLED
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  FastLED.clear();
  updateLocalTime();
   
  if (WiFi.status() == WL_CONNECTED) {
    reconnectWiFi();
    FastLED.clear();
    if (timeClient.update()) {
      lastUpdateTime = timeClient.getEpochTime();
      localSeconds = lastUpdateTime;
      ntpSync = true;
      Serial.println("Sincronización NTP exitosa.");
      FastLED.clear();
    }
  } else {
    ntpSync = false;
    Serial.println("Conexión WiFi perdida, usando tiempo local.");
    FastLED.clear();
  }

  // Convertir localSeconds a hora, minuto y segundo
  unsigned long totalSeconds = localSeconds;
  String hourStr = padZero((totalSeconds % 86400L) / 3600);
  String minuteStr = padZero((totalSeconds % 3600) / 60);
  String secondStr = padZero(totalSeconds % 60);

  // Actualizar la pantalla LED
  updateDisplay(hourStr, minuteStr, secondStr);
}

void updateLocalTime() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    localSeconds++; // Incrementa el tiempo local independientemente de la red
  }
}

void reconnectWiFi() {
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconectando a WiFi...");
    WiFi.begin(); // Reintenta conectar con las credenciales almacenadas
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      //delay(10);
      //Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconexión exitosa");
    } else {
      Serial.println("Falló la reconexión");
    }
  }
  
}

void updateDisplay(String hour, String minute, String second) {
  //FastLED.clear(); // Limpia cualquier LED encendido anteriormente
  drawDigit(hour.charAt(0) - '0', 0, CRGB::White);
  drawDigit(hour.charAt(1) - '0', 64, CRGB::White);
  drawDigit(minute.charAt(0) - '0', 128, CRGB::White);
  drawDigit(minute.charAt(1) - '0', 192, CRGB::White);
  drawColon(second.toInt() % 2 == 0);
  FastLED.show();
}

void drawDigit(int digit, int offset, CRGB color) {
  static const uint8_t PROGMEM numbertable[10][8] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}  // 9
  };

  for (int i = 0; i < 8; i++) {
    uint8_t row = pgm_read_byte(&numbertable[digit][i]);
    for (int j = 0; j < 8; j++) {
      int ledIndex = offset + (i * 8) + j;
      if (row & (1 << (7 - j))) {
        leds[ledIndex] = color;
      } else {
        leds[ledIndex] = CRGB::Black;
      }
    }
  }
}

void drawColon(bool state) {
  int ledIndex1 = 64 + 51; // Primer punto en la columna entre los dígitos de hora y minutos, fila 3
  int ledIndex2 = 64 + 53; // Segundo punto en la columna entre los dígitos de hora y minutos, fila 6
  if (state) {
    leds[ledIndex1] = CRGB::White;
    leds[ledIndex2] = CRGB::White;
  } else {
    leds[ledIndex1] = CRGB::Black;
    leds[ledIndex2] = CRGB::Black;
  }
}

String padZero(int number) {
  if (number < 10) {
    return "0" + String(number);
  }
  return String(number);
}
