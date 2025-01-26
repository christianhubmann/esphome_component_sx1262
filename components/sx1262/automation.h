#pragma once

#include "sx1262.h"

namespace esphome {
namespace sx1262 {

template<typename... Ts> class SendPacketAction : public Action<Ts...>, public Parented<SX1262Component> {
 public:
  void set_data_template(std::function<std::vector<uint8_t>(Ts...)> func) {
    this->data_template_ = func;
    this->simple_ = false;
  }
  void set_data_simple(const std::vector<uint8_t> &data) {
    this->data_simple_ = data;
    this->simple_ = true;
  }

  void play(Ts... x) override {
    if (this->simple_) {
      this->parent_->send_packet(this->data_simple_);
    } else {
      this->parent_->send_packet(this->data_template_(x...));
    }
  }

 protected:
  bool simple_{true};
  std::function<std::vector<uint8_t>(Ts...)> data_template_{};
  std::vector<uint8_t> data_simple_{};
};

class ReceivePacketTrigger : public Trigger<const std::vector<uint8_t> &> {
 public:
  explicit ReceivePacketTrigger(SX1262Component *parent) {
    parent->add_on_packet_receive_callback([this](const std::vector<uint8_t> &data) { this->trigger(data); });
  }
};

}  // namespace sx1262
}  // namespace esphome
