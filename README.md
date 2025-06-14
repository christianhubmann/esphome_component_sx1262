# ESPHome SX1262

ESPHome component for SX1262 LoRa Transceiver.

This component is a wrapper around the [RadioLib](https://github.com/jgromes/RadioLib) library to enable sending and receiving of LoRa packets. Tested with the [Heltec WiFi LoRa 32(V3)](https://heltec.org/project/wifi-lora-32-v3/) board.

Example configuration:

````
external_components:
  - source: github://christianhubmann/esphome_component_sx1262@main
    components: [ sx1262 ]

sx1262:
  id: lora
  clk_pin: 9
  mosi_pin: 10
  miso_pin: 11
  cs_pin: 8
  irq_pin: 14
  reset_pin: 12
  busy_pin: 13
  frequency: 868.5
  on_packet_receive:
    - logger.log:
      level: INFO
      format: "Received message: %s"
      args: [ 'std::string(data.begin(), data.end()).c_str()' ]

script:
  - id: send_message
    mode: single
    then:
      - sx1262.send_packet:
          data: !lambda |-
            std::string s = "Hello World!";
            return std::vector<uint8_t>(s.begin(), s.end());
````

## Limitations

RadioLib handles the SPI initialization, so this component cannot be used together with the [ESPHome SPI Bus](https://esphome.io/components/spi.html). Therefore no other SPI devices can be configured.

So far, this component has only been tested with the [Heltec WiFi LoRa 32(V3)](https://heltec.org/project/wifi-lora-32-v3/) board, but other boards with the SX1262 chip should also work.

## Configuration variables
SPI configuration:
- **clk_pin (Required)**: The pin used for the SPI clock.
- **miso_pin (Required)**: The pin used for the SPI MISO line.
- **moso_pin (Required)**: The pin used for the SPI MOSI line.
- **cs_pin (Required)**: The pin used for chip select. Sometimes also called SS.
- **irq_pin (Required)**: The pin used for interrupt. Sometimes also called DIOn.
- **reset_pin (Required)**:  The pin used for reset.
- **busy_pin (Required)**: The pin used for the busy signal.
- **data_rate** (Optional): Set the data rate of the controller. One of `80MHz`, `40MHz`, `20MHz`, `10MHz`, `5MHz`, `4MHz`, `2MHz`, `1MHz` (default), `200kHz`, `75kHz` or `1kHz`.
- **spi_mode** (Optional): Set the controller mode. One of `0`, `1`, `2`, `3` (default) for MODE0, MODE1, MODE2, MODE3.
- **bit_order** (Optional): Set the bit order. Either `msb_first` (default) or `lsb_first`.

LoRa configuration:
- **frequency (Required)**: The LoRa frequency in MHz, e.g. `434.0`.
- **bandwidth** (Optional): The bandwidth in kHz. One of `125.0` (default), `250.0` and `500.0`.
- **spreading_factor** (Optional): The spreading factor. Allowed values are between `7` and `12`. Defaults to `9`.
- **coding_rate** (Optional): The coding rate. One of `5` for 4/5, `6` for 4/6, `7` (default) for 4/7 and `8` for 4/8.
- **sync_word** (Optional): The sync word. Defaults to `0x12`.
- **power** (Optional): The TX output power in dBm. Defaults to `10`.
- **preamble_length** (Optional): The preamble length. Defaults to `8`.
- **tcxo_voltage** (Optional): TCXO reference voltage in V. Defaults to `1.6`.
- **use_regulator_ldo** (Optional): Whether to use only LDO regulator (`True`) or DC-DC regulator (`False`). Defaults to `False`.

## send_packet Action
Send a LoRa packet. The maximum payload size depends on the LoRa configuration.

````
on_...:
  then:
    - sx1262.send_packet:
        data: !lambda |-
          std::string s = "Hello World!";
          return std::vector<uint8_t>(s.begin(), s.end());
````

Configuration options:
- **data (Required**, std::vector<uint8_t>, templatable): The payload of the packet to be send.
- **blocking** (Optional): Whether send_packet should be a blocking call. Defaults to `False`.

## sleep Action
Put the radio module to sleep.

````
on_...:
  then:
    - sx1262.sleep
````


## on_packet_received Trigger
This automation triggers when a LoRa packet is received. The parameter `data` of type `std::vector<uint8_t>` contains the packet payload.

````
on_packet_receive:
  - logger.log:
    level: INFO
    format: "Received message: %s"
    args: [ 'std::string(data.begin(), data.end()).c_str()' ]
````

## Sensor

````
sensor:
  - platform: sx1262
    rssi:
      id: rssi
      name: "RSSI"
    snr:
      id: snr
      name: "SNR"
````

Configuration variables:
- **rssi** (Optional): The signal strength indication (RSSI) in dBm. Updated after a packet is received.
- **snr** (Optional): The signal-to-noise ratio (SNR) in dB. Updated after a packet is received.

## RadioLib Fork

RadioLib 5.7 added a `library.json` file that breaks the ESPHome build (see [this bug report](https://github.com/jgromes/RadioLib/issues/854) for details). To fix this, [a fork of RadioLib](https://github.com/christianhubmann/esphome-RadioLib) was created and used in this component.
