#include "pxt.h"

using namespace pxt;

// ============================================================
// I2C Slave for MicroBit v1 and v2
// Registers MicroBit as hardware I2C slave
// ============================================================

static uint8_t rxBuffer[32];
static uint8_t txBuffer[32];
static uint8_t lastCmd    = 0;
static bool    hasData    = false;

//% color="#AA278D" icon="\uf085"
//% block="I2C Slave"
namespace i2cSlave {

    /**
     * Initialise I2C slave at given address
     * @param addr I2C slave address e.g. 0x75
     */
    //% blockId="i2cSlave_init"
    //% block="init I2C slave address %addr"
    //% addr.defl=117
    void initSlave(int addr) {
        // Disable master TWI
        NRF_TWI0->ENABLE = 0;
        NRF_TWI1->ENABLE = 0;

        // Configure SDA and SCL pins
        NRF_GPIO->PIN_CNF[20] = 0x0000060C; // SDA = P20
        NRF_GPIO->PIN_CNF[19] = 0x0000060C; // SCL = P19

        // Setup TWIS0
        NRF_TWIS0->PSEL.SDA   = 20;
        NRF_TWIS0->PSEL.SCL   = 19;
        NRF_TWIS0->ADDRESS[0] = (uint8_t)addr;
        NRF_TWIS0->CONFIG     = 1;

        NRF_TWIS0->RXD.PTR    = (uint32_t)rxBuffer;
        NRF_TWIS0->RXD.MAXCNT = 32;
        NRF_TWIS0->TXD.PTR    = (uint32_t)txBuffer;
        NRF_TWIS0->TXD.MAXCNT = 32;

        NRF_TWIS0->SHORTS =
            (1 << 13) | // WRITE_SUSPEND
            (1 << 14);  // READ_SUSPEND

        NRF_TWIS0->EVENTS_WRITE   = 0;
        NRF_TWIS0->EVENTS_READ    = 0;
        NRF_TWIS0->EVENTS_STOPPED = 0;
        NRF_TWIS0->EVENTS_ERROR   = 0;

        NRF_TWIS0->ENABLE = 9; // Enable TWIS
    }

    /**
     * Returns true if new I2C data has arrived
     */
    //% blockId="i2cSlave_dataAvailable"
    //% block="I2C slave data available"
    bool dataAvailable() {
        if (NRF_TWIS0->EVENTS_WRITE) {
            NRF_TWIS0->EVENTS_WRITE = 0;
            NRF_TWIS0->TASKS_RESUME = 1;
        }
        if (NRF_TWIS0->EVENTS_ERROR) {
            NRF_TWIS0->EVENTS_ERROR = 0;
            NRF_TWIS0->TASKS_RESUME = 1;
        }
        if (NRF_TWIS0->EVENTS_STOPPED) {
            NRF_TWIS0->EVENTS_STOPPED = 0;
            if (NRF_TWIS0->RXD.AMOUNT > 0) {
                lastCmd = rxBuffer[0];
                hasData = true;
                NRF_TWIS0->RXD.PTR    = (uint32_t)rxBuffer;
                NRF_TWIS0->RXD.MAXCNT = 32;
            }
        }
        return hasData;
    }

    /**
     * Read last received command byte
     */
    //% blockId="i2cSlave_readByte"
    //% block="I2C slave read byte"
    int readByte() {
        hasData = false;
        return (int)lastCmd;
    }

    /**
     * Write a response byte back to master
     * @param val byte to send
     */
    //% blockId="i2cSlave_writeByte"
    //% block="I2C slave write byte %val"
    //% val.defl=0
    void writeByte(int val) {
        txBuffer[0]           = (uint8_t)val;
        NRF_TWIS0->TXD.PTR    = (uint32_t)txBuffer;
        NRF_TWIS0->TXD.MAXCNT = 1;
    }
}
