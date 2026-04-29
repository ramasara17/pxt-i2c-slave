declare namespace i2cSlave {
    /**
     * Init I2C slave at address
     */
    //% shim=i2cSlave::initSlave
    //% blockId=i2cSlave_init
    //% block="init I2C slave address %addr"
    function initSlave(addr: int32): void;

    /**
     * Returns true if data received
     */
    //% shim=i2cSlave::dataAvailable
    //% blockId=i2cSlave_available
    //% block="I2C data available"
    function dataAvailable(): boolean;

    /**
     * Read received byte
     */
    //% shim=i2cSlave::readByte
    //% blockId=i2cSlave_read
    //% block="I2C read byte"
    function readByte(): int32;

    /**
     * Write byte to master
     */
    //% shim=i2cSlave::writeByte
    //% blockId=i2cSlave_write
    //% block="I2C write byte %val"
    function writeByte(val: int32): void;
}
