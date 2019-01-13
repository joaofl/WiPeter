#include <Arduino.h>
#include <PZEM004T.h>
#include <esphomelib.h>

//Comment this line if you want to have your private
//config is a separete file (that have to be manually created) 
#define USE_EXTERNAL_DEFS

#ifdef USE_EXTERNAL_DEFS
  #include "personal_defs.h"
#else
  #define WIFI_PASS   "your_wifipass"
  #define WIFI_SSID   "your_wifissid"
  #define HA_USERNAME "your_ha_user"
  #define HA_PASS     "your_ha_pass"
  #define HA_IP       "your_ha_ip"
#endif



using namespace esphomelib;


HardwareSerial hwserial(UART0);     // Use hwserial UART0 at pins GPIO1 (TX) and GPIO3 (RX)
PZEM004T pzem(&hwserial);           // Attach PZEM to hwserial
int ip = 0;
bool pzemrdy = false;


void led(bool state){
  digitalWrite(LED_BUILTIN, !state);
}


class PZEMAmpMeter : public sensor::Sensor {
 public:
  PZEMAmpMeter(const std::string &name) : sensor::Sensor(name) {}
  std::string unit_of_measurement() override { return "A"; }
  int8_t accuracy_decimals() override { return 2; }
};

class PZEMVoltMeter : public sensor::Sensor {
 public:
  PZEMVoltMeter(const std::string &name) : sensor::Sensor(name) {}
  std::string unit_of_measurement() override { return "V"; }
  int8_t accuracy_decimals() override { return 1; }
};

class PZEMPowerMeter : public sensor::Sensor {
 public:
  PZEMPowerMeter(const std::string &name) : sensor::Sensor(name) {}
  std::string unit_of_measurement() override { return "W"; }
  int8_t accuracy_decimals() override { return 0; }
};

class PZEMEnergyMeter : public sensor::Sensor {
 public:
  PZEMEnergyMeter(const std::string &name) : sensor::Sensor(name) {}
  std::string unit_of_measurement() override { return "Wh"; }
  int8_t accuracy_decimals() override { return 0; }
};

// SIMPLE EXAMPLE: just pressure
class PZEM : public PollingComponent {
  public:

    PZEMAmpMeter * sensor_A;
    PZEMVoltMeter * sensor_V;
    PZEMPowerMeter * sensor_P;
    PZEMEnergyMeter * sensor_E;


    PZEM(uint32_t update_interval)
        : PollingComponent(update_interval) {
          this->sensor_A = new PZEMAmpMeter("Amp Meter");
          this->sensor_V = new PZEMVoltMeter("Volt Meter");
          this->sensor_P = new PZEMPowerMeter("Power Meter");
          this->sensor_E = new PZEMEnergyMeter("Energy Meter");
        }

    float a, v, p, e;
    uint8_t machine = 0;

    void setup() override {
      // PZEM setup done outside for now
      // this because this setup override sometimes run much later than the
      // update loop itself. Have to clear that.
    }

    void update() override {
      if (!pzemrdy) return;
      //v += 0.0341; a+=0.0265; p += 0.0234; e += 0.0123;

      led(1);

      if (machine == 0){
        v = pzem.voltage(ip);
        sensor_V->publish_state(v);
      }

      if (machine == 1){
        a = pzem.current(ip);
        sensor_A->publish_state(a); 
      }

      if (machine == 2){
        p = pzem.power(ip);
        sensor_P->publish_state(p); 
      }

      if (machine == 3){
        e = pzem.energy(ip);
        sensor_E->publish_state(e); 
      }
  
      machine ++;
      if (machine > 3) {machine = 0;}

      led(0);
    }
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); 
  led(1);

  App.set_name("WiPeter");
  //App.init_log(9600); //Init the serial port at same rate as the one used by the PZEM

  App.init_wifi(WIFI_SSID, WIFI_PASS);
  App.init_ota()->start_safe_mode();
  // Set a plaintext password, alternatively use an MD5 hash for maximum security (set_auth_password_hash)
  // ota->set_auth_plaintext_password("VERY_SECURE");
  // ota->start_safe_mode();

  App.init_mqtt(HA_IP, HA_USERNAME, HA_PASS);

  auto *pzem_module = new PZEM(800);
  App.register_component(pzem_module);
  App.register_sensor(pzem_module->sensor_A);
  App.register_sensor(pzem_module->sensor_V);
  App.register_sensor(pzem_module->sensor_P);
  App.register_sensor(pzem_module->sensor_E);

  App.setup();

  while (!pzemrdy) {
    pzemrdy = pzem.setAddress(ip);
    pzem.setReadTimeout(500); //ms
    delay(500);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  led(0);
}

void loop() {
  App.loop();
}