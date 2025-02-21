#include "sx1262.h"

namespace esphome {
namespace sx1262 {

SX1262Component *cmpt{nullptr};

void set_operation_done(void) {
  if (cmpt) {
    cmpt->operation_done = true;
  }
}

void SX1262Component::dump_config() {
  ESP_LOGCONFIG(TAG, "SX1262:");
  ESP_LOGCONFIG(TAG, "  Pins:");
  ESP_LOGCONFIG(TAG, "    CLK: %d", clk_pin_);
  ESP_LOGCONFIG(TAG, "    MISO: %d", miso_pin_);
  ESP_LOGCONFIG(TAG, "    MOSI: %d", mosi_pin_);
  ESP_LOGCONFIG(TAG, "    CS: %d", cs_pin_);
  ESP_LOGCONFIG(TAG, "    IRQ: %d", irq_pin_);
  ESP_LOGCONFIG(TAG, "    RESET: %d", reset_pin_);
  ESP_LOGCONFIG(TAG, "    BUSY: %d", busy_pin_);
  ESP_LOGCONFIG(TAG, "  LoRa:");
  ESP_LOGCONFIG(TAG, "    Frequency: %.3f MHz", frequency_);
  ESP_LOGCONFIG(TAG, "    Bandwidth: %.3f kHz", bandwidth_);
  ESP_LOGCONFIG(TAG, "    Spreading Factor: %d", spreading_factor_);
  ESP_LOGCONFIG(TAG, "    Coding Rate: %d", coding_rate_);
  ESP_LOGCONFIG(TAG, "    Sync Word: 0x%02X", sync_word_);
  ESP_LOGCONFIG(TAG, "    TX Power: %d dBm", tx_power_);
  ESP_LOGCONFIG(TAG, "    Preamble Length: %d", preamble_length_);
#ifdef USE_SENSOR
  LOG_SENSOR("  ", "RSSI", rssi_sensor_);
  LOG_SENSOR("  ", "SNR", snr_sensor_);
#endif

  if (init_state_ != RADIOLIB_ERR_NONE) {
    ESP_LOGE(TAG, "Communication with SX1262 failed: %d", init_state_);
  }
}

void SX1262Component::setup() {
  cmpt = this;
  ESP_LOGI(TAG, "Initializing...");

  SPI.begin(clk_pin_, miso_pin_, mosi_pin_);
  SPI.setFrequency(this->data_rate_);
  SPI.setDataMode(this->spi_mode_);
  SPI.setBitOrder(this->bit_order_);

  radio_ = new SX1262(new Module(cs_pin_, irq_pin_, reset_pin_, busy_pin_));

  init_state_ = radio_->begin(this->frequency_, this->bandwidth_, this->spreading_factor_, this->coding_rate_,
                              this->sync_word_, this->tx_power_, this->preamble_length_);

  if (init_state_ == RADIOLIB_ERR_NONE) {
    ESP_LOGI(TAG, "Success.");
  } else {
    ESP_LOGE(TAG, "Failed: %d", init_state_);
    return;
  }

  radio_->setPacketReceivedAction(set_operation_done);
  radio_->setPacketSentAction(set_operation_done);
  radio_->startReceive();
}

void SX1262Component::loop() {
  if (operation_done) {
    operation_done = false;

    if (is_transmitting_) {
      // Sending of a packet has finished.
      is_transmitting_ = false;

      if (transmission_state_ == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG, "Send OK");
      } else {
        ESP_LOGE(TAG, "Send failed: %d", transmission_state_);
      }

      radio_->finishTransmit();
      radio_->startReceive();
    } else {
      // Receiving of a packet has finished.
      const size_t len = radio_->getPacketLength();
      if (len > 0 && len <= MAX_PACKET_LENGTH) {
        std::vector<uint8_t> data(len);
        const int16_t read_state = radio_->readData(data.data(), len);

        if (read_state == RADIOLIB_ERR_NONE) {
          ESP_LOGI(TAG, "Receive OK, length: %d", len);
          const float rssi = radio_->getRSSI();
          const float snr = radio_->getSNR();
          ESP_LOGV(TAG, "RSSI: %.0f dBm", rssi);
          ESP_LOGV(TAG, "SNR: %.0f dB", snr);
#ifdef USE_SENSOR
          if (this->rssi_sensor_) {
            this->rssi_sensor_->publish_state(rssi);
          }
          if (this->snr_sensor_) {
            this->snr_sensor_->publish_state(snr);
          }
#endif
          this->on_packet_receive_callback_.call(data);
        } else {
          ESP_LOGE(TAG, "Receive failed: %d", read_state);
        }
      } else {
        ESP_LOGE(TAG, "Invalid received packet length: %d", len);
      }

      // A transmit might have started in the receive callback.
      // In that case, do not start receiving to not abort the transmit.
      if (!is_transmitting_) {
        radio_->startReceive();
      }
    }
  }
}

void SX1262Component::send_packet(const std::vector<uint8_t> &data) {
  if (init_state_ != RADIOLIB_ERR_NONE) {
    ESP_LOGE(TAG, "Not initialized: %d", init_state_);
    return;
  }
  if (data.size() > MAX_PACKET_LENGTH) {
    ESP_LOGE(TAG, "Packet too large: %d", data.size());
    return;
  }

  ESP_LOGV(TAG, "Sending packet, length %d", data.size());
  is_transmitting_ = true;
  transmission_state_ = radio_->startTransmit(data.data(), data.size());
}

}  // namespace sx1262
}  // namespace esphome
