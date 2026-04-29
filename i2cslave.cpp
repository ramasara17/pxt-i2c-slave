#include "pxt.h"
#include "nrf.h"

static uint8_t  _rxBuf[4];
static uint8_t  _txBuf[4];
static uint8_t  _lastRx  = 0;
static bool     _hasData = false;

static void poll() {
    if (NRF_TWIS0->EVENTS_WRITE) {
        NRF_TWIS0->EVENTS_WRITE = 0;
        NRF_TWIS0->TASKS_RESUME = 1;
    }
    if (NRF_TWIS0->EVENTS_ERROR) {
        NRF_TWIS0->EVENTS_ERROR  = 0;
        NRF_TWIS0->ERRORSRC      = 0xFFFFFFFF;
        NRF_TWIS0->TASKS_RESUME  = 1;
    }
    if (NRF_TWIS0->EVENTS_READ) {
        NRF_TWIS0->EVENTS_READ   = 0;
        NRF_TWIS0->TASKS_RESUME  = 1;
    }
    if (NRF_TWIS0->EVENTS_STOPPED) {
        NRF_TWIS0->EVENTS_STOPPED = 0;
        uint32_t n = NRF_TWIS0->RXD.AMOUNT;
        if (n > 0) {
            _lastRx  = _rxBuf[0];
            _hasData = true;
            NRF_TWIS0->RXD.PTR    = (uint32_t)_rxBuf;
            NRF_TWIS0->RXD.MAXCNT = 4;
        }
    }
}

namespace i2cSlave {

    //%
    void initSlave(int addr) {
        // disable master TWI
        NRF_TWI0->ENABLE  = 0;
        NRF_TWI1->ENABLE  = 0;
        NRF_TWIS0->ENABLE = 0;

        // SDA = P20, SCL = P19 on MicroBit
        NRF_GPIO->PIN_CNF[20] = (GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos)
                               | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                               | (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)
                               | (GPIO_PIN_CNF_DRIVE_S0D1    << GPIO_PIN_CNF_DRIVE_Pos);

        NRF_GPIO->PIN_CNF[19] = (GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos)
                               | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                               | (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)
                               | (GPIO_PIN_CNF_DRIVE_S0D1    << GPIO_PIN_CNF_DRIVE_Pos);

        NRF_TWIS0->PSEL.SDA    = 20;
        NRF_TWIS0->PSEL.SCL    = 19;
        NRF_TWIS0->ADDRESS[0]  = (uint8_t)(addr & 0x7F);
        NRF_TWIS0->CONFIG      = TWIS_CONFIG_ADDRESS0_Enabled
                                  << TWIS_CONFIG_ADDRESS0_Pos;

        NRF_TWIS0->RXD.PTR     = (uint32_t)_rxBuf;
        NRF_TWIS0->RXD.MAXCNT  = 4;
        NRF_TWIS0->TXD.PTR     = (uint32_t)_txBuf;
        NRF_TWIS0->TXD.MAXCNT  = 4;

        NRF_TWIS0->SHORTS      = (TWIS_SHORTS_WRITE_SUSPEND_Enabled
                                   << TWIS_SHORTS_WRITE_SUSPEND_Pos)
                                | (TWIS_SHORTS_READ_SUSPEND_Enabled
                                   << TWIS_SHORTS_READ_SUSPEND_Pos);

        NRF_TWIS0->EVENTS_STOPPED = 0;
        NRF_TWIS0->EVENTS_ERROR   = 0;
        NRF_TWIS0->EVENTS_WRITE   = 0;
        NRF_TWIS0->EVENTS_READ    = 0;

        NRF_TWIS0->ENABLE = TWIS_ENABLE_ENABLE_Enabled
                             << TWIS_ENABLE_ENABLE_Pos;
    }

    //%
    bool dataAvailable() {
        poll();
        return _hasData;
    }

    //%
    int readByte() {
        _hasData = false;
        return (int)_lastRx;
    }

    //%
    void writeByte(int val) {
        _txBuf[0]             = (uint8_t)(val & 0xFF);
        NRF_TWIS0->TXD.PTR    = (uint32_t)_txBuf;
        NRF_TWIS0->TXD.MAXCNT = 1;
    }
}
