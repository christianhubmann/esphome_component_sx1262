#pragma once

#include "sx1262.h"

namespace esphome {
namespace sx1262 {

template<typename... Ts> class SendPacketAction : public Action<Ts...>, public Parented<SX1262Component> {
 public:
  void set_data_template(std::function<std::vector<uint8_t>(Ts...)> func) {
    this->data_func_ = func;
    this->static_ = false;
  }

  void set_data_static(const std::vector<uint8_t> &data) {
    this->data_static_ = data;
    this->static_ = true;
  }

  void set_blocking(const bool blocking) { this->blocking_ = blocking; }

  void play(Ts... x) override {
    if (this->static_) {
      this->parent_->send_packet(this->data_static_, this->blocking_);
    } else {
      auto val = this->data_func_(x...);
      this->parent_->send_packet(val, this->blocking_);
    }
  }

 protected:
  bool static_{false};
  std::function<std::vector<uint8_t>(Ts...)> data_func_{};
  std::vector<uint8_t> data_static_{};
  bool blocking_{false};
};

class ReceivePacketTrigger : public Trigger<const std::vector<uint8_t> &> {
 public:
  explicit ReceivePacketTrigger(SX1262Component *parent) {
    parent->add_on_packet_receive_callback([this](const std::vector<uint8_t> &data) { this->trigger(data); });
  }
};

}  // namespace sx1262
}  // namespace esphome
