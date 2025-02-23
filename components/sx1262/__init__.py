from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_BUSY_PIN,
    CONF_CLK_PIN,
    CONF_CS_PIN,
    CONF_DATA,
    CONF_DATA_RATE,
    CONF_FREQUENCY,
    CONF_ID,
    CONF_IRQ_PIN,
    CONF_MISO_PIN,
    CONF_MOSI_PIN,
    CONF_RESET_PIN,
    CONF_TRIGGER_ID,
    CONF_TX_POWER,
)

DEPENDENCIES = []
CONFLICTS_WITH = ["spi"]
CODEOWNERS = ["@christianhubmann"]

CONF_SPI_MODE = "spi_mode"
CONF_BIT_ORDER = "bit_order"
CONF_BANDWIDTH = "bandwidth"
CONF_SPREADING_FACTOR = "spreading_factor"
CONF_CODING_RATE = "coding_rate"
CONF_SYNC_WORD = "sync_word"
CONF_PREAMBLE_LENGTH = "preamble_length"
CONF_ON_PACKET_RECEIVE = "on_packet_receive"
CONF_BLOCKING = "blocking"

sx1262_ns = cg.esphome_ns.namespace("sx1262")
SX1262Component = sx1262_ns.class_(
  "SX1262Component", cg.Component)

vector_uint8_t = cg.std_vector.template(cg.uint8)
vector_uint8_const_ref_t = vector_uint8_t.operator("ref").operator("const")

SendPacketAction = sx1262_ns.class_("SendPacketAction", automation.Action)
SleepAction = sx1262_ns.class_("SleepAction", automation.Action)

ReceivePacketTrigger = sx1262_ns.class_(
  "ReceivePacketTrigger",
  automation.Trigger.template(vector_uint8_const_ref_t)
)


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SX1262Component),

            # SPI settings
            cv.Required(CONF_CLK_PIN): cv.int_,
            cv.Required(CONF_MISO_PIN): cv.int_,
            cv.Required(CONF_MOSI_PIN): cv.int_,
            cv.Required(CONF_CS_PIN): cv.int_,
            cv.Required(CONF_IRQ_PIN): cv.int_,
            cv.Required(CONF_RESET_PIN): cv.int_,
            cv.Optional(CONF_BUSY_PIN): cv.int_,
            cv.Optional(CONF_DATA_RATE, default=1e6): cv.All(cv.frequency, cv.one_of(
                80e6, 40e6, 20e6, 10e6, 8e6, 5e6, 4e6, 2e6, 1e6, 2e5, 75e3, 1e3
            )),
            cv.Optional(CONF_SPI_MODE, default=3): cv.one_of(0, 1, 2, 3),
            cv.Optional(CONF_BIT_ORDER, default="msb_first"): cv.enum({
                "lsb_first": 0,
                "msb_first": 1
            }, lower=True),

            # LoRa settings
            cv.Required(CONF_FREQUENCY): cv.float_,
            cv.Optional(CONF_BANDWIDTH, default=125.0): cv.one_of(125.0, 250.0, 500.0),
            cv.Optional(CONF_SPREADING_FACTOR, default=9): cv.one_of(6, 7, 8, 9, 10, 11, 12),
            cv.Optional(CONF_CODING_RATE, default=7): cv.one_of(5, 6, 7, 8),
            cv.Optional(CONF_SYNC_WORD, default=0x12): cv.hex_uint8_t,
            cv.Optional(CONF_TX_POWER, default=10): cv.int_,
            cv.Optional(CONF_PREAMBLE_LENGTH, default=8): cv.int_,

            # Automation
            cv.Optional(CONF_ON_PACKET_RECEIVE): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReceivePacketTrigger),
                }
            )
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    cg.add_library("SPI", None)
    # Use fork of RadioLib to fix esphome build
    # cg.add_library("jgromes/RadioLib", "7.1.2")
    cg.add_library(
      name="RadioLib",
      repository="https://github.com/christianhubmann/esphome-RadioLib.git#7.1.2",
      version="7.1.2",
    )
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_CLK_PIN in config:
        cg.add(var.set_clk_pin(config[CONF_CLK_PIN]))
    if CONF_MISO_PIN in config:
        cg.add(var.set_miso_pin(config[CONF_MISO_PIN]))
    if CONF_MOSI_PIN in config:
        cg.add(var.set_mosi_pin(config[CONF_MOSI_PIN]))
    if CONF_CS_PIN in config:
        cg.add(var.set_cs_pin(config[CONF_CS_PIN]))
    if CONF_IRQ_PIN in config:
        cg.add(var.set_irq_pin(config[CONF_IRQ_PIN]))
    if CONF_RESET_PIN in config:
        cg.add(var.set_reset_pin(config[CONF_RESET_PIN]))
    if CONF_BUSY_PIN in config:
        cg.add(var.set_busy_pin(config[CONF_BUSY_PIN]))
    if CONF_DATA_RATE in config:
        cg.add(var.set_data_rate(config[CONF_DATA_RATE]))
    if CONF_SPI_MODE in config:
        cg.add(var.set_spi_mode(config[CONF_SPI_MODE]))
    if CONF_BIT_ORDER in config:
        cg.add(var.set_bit_order(config[CONF_BIT_ORDER]))

    if CONF_FREQUENCY in config:
        cg.add(var.set_frequency(config[CONF_FREQUENCY]))
    if CONF_BANDWIDTH in config:
        cg.add(var.set_bandwidth(config[CONF_BANDWIDTH]))
    if CONF_SPREADING_FACTOR in config:
        cg.add(var.set_spreading_factor(config[CONF_SPREADING_FACTOR]))
    if CONF_CODING_RATE in config:
        cg.add(var.set_coding_rate(config[CONF_CODING_RATE]))
    if CONF_SYNC_WORD in config:
        cg.add(var.set_sync_word(config[CONF_SYNC_WORD]))
    if CONF_TX_POWER in config:
        cg.add(var.set_tx_power(config[CONF_TX_POWER]))
    if CONF_PREAMBLE_LENGTH in config:
        cg.add(var.set_preamble_length(config[CONF_PREAMBLE_LENGTH]))

    for conf in config.get(CONF_ON_PACKET_RECEIVE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(vector_uint8_const_ref_t, "data")], conf)


SEND_PACKET_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(SX1262Component),
        cv.Required(CONF_DATA): cv.templatable(cv.ensure_list(cv.hex_uint8_t)),
        cv.Optional(CONF_BLOCKING, default=False): cv.boolean,
    }
)

SLEEP_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(SX1262Component)
    }
)


@automation.register_action(
    "sx1262.send_packet", SendPacketAction, SEND_PACKET_SCHEMA
)
async def send_packet_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])

    data = config[CONF_DATA]
    if cg.is_template(data):
        templ = await cg.templatable(data, args, cg.std_vector.template(cg.uint8))
        cg.add(var.set_data_template(templ))
    else:
        cg.add(var.set_data_static(data))

    if CONF_BLOCKING in config:
        cg.add(var.set_blocking(config[CONF_BLOCKING]))

    return var


@automation.register_action(
    "sx1262.sleep", SleepAction, SLEEP_SCHEMA
)
async def sleep_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var
