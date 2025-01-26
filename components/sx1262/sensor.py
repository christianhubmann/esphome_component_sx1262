import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    ENTITY_CATEGORY_DIAGNOSTIC,
    STATE_CLASS_MEASUREMENT,
    UNIT_DECIBEL,
    UNIT_DECIBEL_MILLIWATT,
)

from . import SX1262Component

DEPENDENCIES = ["sx1262"]

CONF_RSSI = "rssi"
CONF_SNR = "snr"

CONFIG_SCHEMA = {
    cv.GenerateID(): cv.use_id(SX1262Component),
    cv.Optional(CONF_RSSI): sensor.sensor_schema(
        unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    ),
    cv.Optional(CONF_SNR): sensor.sensor_schema(
        unit_of_measurement=UNIT_DECIBEL,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )
}


async def to_code(config):
    sx1262_component = await cg.get_variable(config[CONF_ID])

    for key in [
        CONF_RSSI,
        CONF_SNR
    ]:
        if key not in config:
            continue
        sens = await sensor.new_sensor(config[key])
        cg.add(getattr(sx1262_component, f"set_{key}_sensor")(sens))
