#include <DHT.h>

#define DHTPIN   2      // Pin donde conectaste DATA del DHT22
#define DHTTYPE  DHT22

#define HUM_OUT_PIN 3   // Pin que va a la Zybo (a través del divisor)

const float HUM_THRESHOLD = 60.0;  // % de humedad para disparar

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(HUM_OUT_PIN, OUTPUT);
  digitalWrite(HUM_OUT_PIN, LOW); // Por defecto, no hay orgánico
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Error leyendo DHT");
    digitalWrite(HUM_OUT_PIN, LOW);   // Por seguridad
    delay(2000);
    return;
  }

  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %  Temp: ");
  Serial.println(t);

  // Si supera el umbral, ponemos la salida en 1
  if (h > HUM_THRESHOLD) {
    digitalWrite(HUM_OUT_PIN, HIGH);
  } else {
    digitalWrite(HUM_OUT_PIN, LOW);
  }

  delay(2000);  // Lee cada 2 s
}
