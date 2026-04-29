/**
 * I2C Slave extension for MicroBit
 */

//% weight=100 color="#AA278D" icon="\uf085" block="I2CSlave"
namespace i2cSlave {

    let _addr = 0
    let _lastByte = 0
    let _hasData = false

    /**
     * Init I2C slave at address
     * @param addr slave address e.g. 117
     */
    //% blockId="i2cSlave_init"
    //% block="init I2C slave at address %addr"
    //% addr.defl=117
    export function initSlave(addr: number): void {
        _addr = addr
        _hasData = false
        _lastByte = 0
        serial.writeLine("i2cSlave init at " + addr)
    }

    /**
     * Check if data is available
     */
    //% blockId="i2cSlave_available"
    //% block="I2C data available"
    export function dataAvailable(): boolean {
        let buf = pins.i2cReadBuffer(_addr, 1, false)
        if (buf.length > 0) {
            let b = buf.getNumber(NumberFormat.UInt8LE, 0)
            if (b > 0) {
                _lastByte = b
                _hasData = true
            }
        }
        return _hasData
    }

    /**
     * Read last received byte
     */
    //% blockId="i2cSlave_read"
    //% block="I2C read byte"
    export function readByte(): number {
        _hasData = false
        return _lastByte
    }

    /**
     * Write byte back to master
     * @param val byte to send
     */
    //% blockId="i2cSlave_write"
    //% block="I2C write byte %val"
    //% val.defl=0
    export function writeByte(val: number): void {
        let buf = pins.createBuffer(1)
        buf.setNumber(NumberFormat.UInt8LE, 0, val)
        pins.i2cWriteBuffer(_addr, buf, false)
    }
}
