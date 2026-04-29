#include "pxt.h"
#include "MicroBit.h"
#include "MicroBitI2C.h"

// ============================================================
// MicroBit I2C Slave implementation
// Uses nRF51/nRF52 TWI peripheral in slave mode
// ============================================================

extern "C" {
#include "nrf.h"
}

#define I2C_SLAVE_BUFFER_SIZE 32

static uint8_t  rxBuffer[I2C_SLAVE_BUFFER_SIZE];
static uint8_t  txBuffer[I2C_SLAVE_BUFFER_SIZE];
static uint8_t  rxLen       = 0;
static uint8_t  txLen       = 0;
static bool     dataReady   = false;
static uint8_t  slaveAddr   = 0x75;

// ── nRF TWI slave init ──────────────────────────────────────
static void i2c_slave_init(uint8_t addr) {
    // Disable TWI master first
    NRF_TWI0->ENABLE = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
    NRF_TWI1->ENABLE = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;

    // Configure GPIO pins for I2C (P19=SCL, P20=SDA on MicroBit)
    NRF_GPIO->PIN_CNF[MICROBIT_PIN_SDA] =
        (GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos)   |
        (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
        (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)  |
        (GPIO_PIN_CNF_DRIVE_S0D1    << GPIO_PIN_CNF_DRIVE_Pos) |
        (GPIO_PIN_CNF_SENSE_Disabled<< GPIO_PIN_CNF_SENSE_Pos);

    NRF_GPIO->PIN_CNF[MICROBIT_PIN_SCL] =
        (GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos)   |
        (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
        (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)  |
        (GPIO_PIN_CNF_DRIVE_S0D1    << GPIO_PIN_CNF_DRIVE_Pos) |
        (GPIO_PIN_CNF_SENSE_Disabled<< GPIO_PIN_CNF_SENSE_Pos);

    // Setup TWIS (TWI Slave) on nRF52
    NRF_TWIS0->PSEL.SCL  = MICROBIT_PIN_SCL;
    NRF_TWIS0->PSEL.SDA  = MICROBIT_PIN_SDA;
    NRF_TWIS0->ADDRESS[0]= addr;
    NRF_TWIS0->CONFIG    = TWIS_CONFIG_ADDRESS0_Enabled << TWIS_CONFIG_ADDRESS0_Pos;

    // Set RX/TX buffers
    NRF_TWIS0->RXD.PTR    = (uint32_t)rxBuffer;
    NRF_TWIS0->RXD.MAXCNT = I2C_SLAVE_BUFFER_SIZE;
    NRF_TWIS0->TXD.PTR    = (uint32_t)txBuffer;
    NRF_TWIS0->TXD.MAXCNT = I2C_SLAVE_BUFFER_SIZE;

    // Clear events
    NRF_TWIS0->EVENTS_WRITE   = 0;
    NRF_TWIS0->EVENTS_READ    = 0;
    NRF_TWIS0->EVENTS_STOPPED = 0;
    NRF_TWIS0->EVENTS_ERROR   = 0;

    // Enable shortcuts: WRITE→SUSPEND, READ→SUSPEND
    NRF_TWIS0->SHORTS =
        (TWIS_SHORTS_WRITE_SUSPEND_Enabled << TWIS_SHORTS_WRITE_SUSPEND_Pos) |
        (TWIS_SHORTS_READ_SUSPEND_Enabled  << TWIS_SHORTS_READ_SUSPEND_Pos);

    // Enable TWIS
    NRF_TWIS0->ENABLE =
        TWIS_ENABLE_ENABLE_Enabled << TWIS_ENABLE_ENABLE_Pos;
}

// ── Poll for received data ──────────────────────────────────
static void i2c_slave_poll() {
    if (NRF_TWIS0->EVENTS_WRITE) {
        NRF_TWIS0->EVENTS_WRITE = 0;
        NRF_TWIS0->TASKS_RESUME = 1;
    }
    if (NRF_TWIS0->EVENTS_STOPPED) {
        NRF_TWIS0->EVENTS_STOPPED = 0;
        rxLen     = NRF_TWIS0->RXD.AMOUNT;
        dataReady = (rxLen > 0);
        // Reset RX buffer for next transaction
        NRF_TWIS0->RXD.PTR    = (uint32_t)rxBuffer;
        NRF_TWIS0->RXD.MAXCNT = I2C_SLAVE_BUFFER_SIZE;
    }
    if (NRF_TWIS0->EVENTS_ERROR) {
        NRF_TWIS0->EVENTS_ERROR   = 0;
        NRF_TWIS0->TASKS_RESUME   = 1;
    }
}

// ============================================================
// MakeCode / PXT exported functions
// ============================================================
namespace i2cSlave {

    // Initialise I2C slave at given address
    //%  blockId="i2c_slave_init"
    //%  block="init I2C slave at address %addr"
    void initSlave(int addr) {
        slaveAddr = (uint8_t)addr;
        i2c_slave_init(slaveAddr);
    }

    // Returns true if new data has arrived
    //%  blockId="i2c_slave_data_ready"
    //%  block="I2C slave data ready"
    bool dataAvailable() {
        i2c_slave_poll();
        return dataReady;
    }

    // Read the last received byte
    //%  blockId="i2c_slave_read_byte"
    //%  block="I2C slave read byte"
    int readByte() {
        i2c_slave_poll();
        if (dataReady && rxLen > 0) {
            dataReady = false;
            return rxBuffer[0];
        }
        return -1;
    }

    // Read full received buffer as a string
    //%  blockId="i2c_slave_read_buffer"
    //%  block="I2C slave read buffer"
    String readBuffer() {
        i2c_slave_poll();
        if (dataReady) {
            dataReady = false;
            return mkString((char*)rxBuffer, rxLen);
        }
        return mkString("", 0);
    }

    // Write response byte (for master read requests)
    //%  blockId="i2c_slave_write_byte"
    //%  block="I2C slave write byte %val"
    void writeByte(int val) {
        txBuffer[0] = (uint8_t)val;
        txLen = 1;
        NRF_TWIS0->TXD.PTR    = (uint32_t)txBuffer;
        NRF_TWIS0->TXD.MAXCNT = txLen;
    }
}
