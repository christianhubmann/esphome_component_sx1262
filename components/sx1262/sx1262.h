#pragma once

#include "esphome/core/helpers.h"
#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/core/automation.h"
#include "esphome/core/hal.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#include <RadioLib.h>
#include <SPI.h>

namespace esphome {
namespace sx1262 {

static const char *const TAG = "sx1262";
// Hard limit for packet length. The actual maximum length depends on the configured bandwidth and spreading factor.
static const size_t MAX_PACKET_LENGTH = 256;

class SX1262Component : public Component {
 public:
  void dump_config() override;
  void setup() override;
  void loop() override;
  void set_clk_pin(int8_t clk_pin) { this->clk_pin_ = clk_pin; }
  void set_miso_pin(int8_t miso_pin) { this->miso_pin_ = miso_pin; }
  void set_mosi_pin(int8_t mosi_pin) { this->mosi_pin_ = mosi_pin; }
  void set_cs_pin(int8_t cs_pin) { this->cs_pin_ = cs_pin; }
  void set_irq_pin(int8_t irq_pin) { this->irq_pin_ = irq_pin; }
  void set_reset_pin(int8_t reset_pin) { this->reset_pin_ = reset_pin; }
  void set_busy_pin(int8_t busy_pin) { this->busy_pin_ = busy_pin; }
  void set_data_rate(uint32_t data_rate) { this->data_rate_ = data_rate; }
  void set_spi_mode(uint8_t spi_mode) { this->spi_mode_ = spi_mode; }
  void set_bit_order(uint8_t bit_order) { this->bit_order_ = bit_order; }
  void set_frequency(float frequency) { this->frequency_ = frequency; }
  void set_bandwidth(float bandwidth) { this->bandwidth_ = bandwidth; }
  void set_spreading_factor(uint8_t spreading_factor) { this->spreading_factor_ = spreading_factor; }
  void set_coding_rate(uint8_t coding_rate) { this->coding_rate_ = coding_rate; }
  void set_sync_word(uint8_t sync_word) { this->sync_word_ = sync_word; }
  void set_tx_power(int8_t tx_power) { this->tx_power_ = tx_power; }
  void set_preamble_length(uint16_t preamble_length) { this->preamble_length_ = preamble_length; }
  void send_packet(const std::vector<uint8_t> &data, const bool blocking = false);
  void add_on_packet_receive_callback(std::function<void(const std::vector<uint8_t> &)> callback) {
    this->on_packet_receive_callback_.add(std::move(callback));
  }
#ifdef USE_SENSOR
  void set_rssi_sensor(sensor::Sensor *sensor) { rssi_sensor_ = sensor; }
  void set_snr_sensor(sensor::Sensor *sensor) { snr_sensor_ = sensor; }
#endif
  bool operation_done{false};

 protected:
  SX1262 *radio_;
  int16_t init_state_;
  int8_t clk_pin_{-1};
  int8_t miso_pin_{-1};
  int8_t mosi_pin_{-1};
  int8_t cs_pin_{-1};
  int8_t irq_pin_{-1};
  int8_t reset_pin_{-1};
  int8_t busy_pin_{-1};
  uint32_t data_rate_{1000000};
  uint8_t spi_mode_{3};
  uint8_t bit_order_{1};
  float frequency_{434.0};
  float bandwidth_{125.0};
  uint8_t spreading_factor_{9};
  uint8_t coding_rate_{7};
  uint8_t sync_word_{0x12};
  int8_t tx_power_{10};
  uint16_t preamble_length_{8};
  CallbackManager<void(const std::vector<uint8_t> &)> on_packet_receive_callback_;
#ifdef USE_SENSOR
  sensor::Sensor *rssi_sensor_{nullptr};
  sensor::Sensor *snr_sensor_{nullptr};
#endif
  bool is_transmitting_;
  int16_t transmission_state_;
};
}  // namespace sx1262
}  // namespace esphome
