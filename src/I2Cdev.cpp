/**
 * I2Cdev 实现类
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include "I2Cdev.h"

/**
 * 默认构造函数
 */
I2Cdev::I2Cdev(){};
/**
 * 从一个8位设备寄存器读取一位数据
 * @param  devAddr I2C设备地址
 * @param  regAddr 要读取数据的寄存器地址
 * @param  bitNum  读取第几位数据(0-7)
 * @param  data    该位的数据(返回值)
 * @param  timeout 操作超时的时间
 * @return         读操作的状态
 */
int8_t I2Cdev::readBit(uint8_t devAddr,uint8_t regAddr,uint8_t bitNum,uint8_t *data,uint16_t timeout)
{
    uint8_t b;
    uint8_t count = readByte(devAddr,regAddr,&b,timeout);

    *data = b & (1 << bitNum);

    return count;
}
/**
 * 从一个16位寄存器中读取一位数据
 * @param  devAddr I2C设备地址
 * @param  regAddr 要读取数据的寄存器地址
 * @param  bitNum  读取第几位数据(0-15)
 * @param  data    读取到的数据
 * @param  timeout 超时
 * @return         读取状态
 */
int8_t I2Cdev::readBitW(uint8_t devAddr,uint8_t regAddr,uint8_t bitNum,uint16_t *data,uint16_t timeout)
{
    uint16_t b;
    uint8_t count = readWord(devAddr,regAddr,&b,timeout);

    *data = b << (1 << bitNum);

    return count;
}
/**
 * 从一个16位寄存器中读取多位数据
 * @param  devAddr  I2C设备地址
 * @param  regAddr  要读取数据的寄存器地址
 * @param  bitStart 读取数据的起始位(0-15)
 * @param  length   要读取数据的长度
 * @param  data     读取到的数据
 * @param  timeout  超时
 * @return          读取状态
 */
int8_t I2Cdev::readBits(uint8_t devAddr,uint8_t regAddr,uint8_t bitStart,uint8_t length,uint8_t *data,uint16_t timeout)
{
    uint8_t count,b;
    count = readByte(devAddr,regAddr,&b,timeout);
    if(count > 0)
    {
        uint8_t mask = (1 << length - 1) << bitStart;
        b &= mask;
        b >> bitStart
        *data = b;
    }
    return count;
}
/**
 * 从16位寄存器中读取多个word(mpu6050为16位)
 * @param  devAddr  I2C设备地址
 * @param  regAddr  要读取数据的寄存器地址
 * @param  bitStart 读取数据的起始位(0-15)
 * @param  length   要读取数据的长度
 * @param  data     读取到的数据
 * @param  timeout  超时
 * @return          读取状态
 */
int8_t I2Cdev::readBitsW(uint8_t devAddr,uint8_t regAddr,uint8_t bitStart,uint8_t length,uint16_t *data,uint16_t timeout)
{
    uint8_t count;
    uint16_t w;
    count = readWord(devAddr,regAddr,&w,timeout);
    if(count >= 0)
    {
        uint16_t mask = ((1 << length - 1)) << bitStart;
        w &= mask;
        w >> bitStart;

        *data = w;
    }

    return count;
}
/**
 * 从一个8位寄存器中读取一个byte数据
 * @param  devAddr I2C设备地址
 * @param  regAddr 要读取数据的寄存器地址
 * @param  data    读取到的数据
 * @param  timeout 超时
 * @return         读取数据的状态
 */
int8_t I2Cdev::readByte(uint8_t devAddr,uint8_t regAddr,uint8_t *data,uint16_t timeout)
{
    return readBytes(devAddr,regAddr,1,data,timeout);
}
/**
 * 读取单个word(mpu6050中为16位)数据
 * @param  devAddr I2C设备地址
 * @param  regAddr 要读取数据的寄存器地址
 * @param  data    读取到的数据
 * @param  timeout 超时
 * @return         读取数据的状态
 */
int8_t I2Cdev::readWord(uint8_t devAddr,uint8_t regAddr,uint16_t *data,uint16_t timeout)
{
    return readWords(devAddr,regAddr,1,data,timeout);
}
/**
 * 从一个8位寄存器中读取多个bytes
 * @param  devAddr I2C设备地址
 * @param  regAddr 寄存器地址
 * @param  length  读取数据的长度
 * @param  data    读取到的数据存储地址
 * @param  timeout 超时
 * @return         读取到数据的个数
 */
int8_t I2Cdev::readBytes(uint8_t devAddr,uint8_t regAddr,uint8_t length,uint8_t *data,uint16_t timeout)
{
    int8_t count = 0;
    int fd = open("/dev/i2c-1",O_RDWR);
    if(fd < 0)
    {
        fprintf(stderr, "Failed to open device:%s\n",strerror(errno));
        return -1;
    }

    if(ioctl(fd,I2C_SLAVE,devAddr) < 0)
    {
        fprintf(stderr, " Failed to select device:%s\n",strerror(errno));
        //关闭打开的文件描述符
        close(fd);
        return -1;
    }

    count = read(fd,data,length);
    if(count < 0)
    {
        fprintf(stderr, "Failed to read device(%d):%s\n",count,::strerror(errno));
        close(fd);
        return -1;
    }
    else if(count != length)
    {
        fprintf(stderr, "Short read from device,expected %d, got %d\n",length,count);
        close(fd);
        return -1;
    }
    close(fd);

    return count;
}
/**
 * 读取多个word(mpu6050中为多个16位数据)
 * @param  devAddr I2C设备地址
 * @param  regAddr 寄存器地址
 * @param  length  读取数据的长度
 * @param  data    读取到的数据存储地址
 * @param  timeout 超时
 * @return         读取到数据的个数
 */
int8_t I2Cdev::readWords(uint8_t devAddr,uint8_t regAddr,uint8_t length,uint16_t *data,uint16_t timeout)
{
    int8_t count = 0;
    uint8_t buf[128];

    if(length > 63)
    {
        fprintf(stderr, "Word read count (%d) > 63\n", length);
        return -1;
    }

    count = readBytes(devAddr,regAddr,length*2,buf,timeout);
    for(int i = 0; i < length * 2; i++)
    {
        data[i] = buf[i*2] << 8;  //高位赋值
        data[i] |= buf[i*2+1];    //低位赋值
    }

    return count;
}

/**
 * 写8位寄存器中的一位
 * @param  devAddr I2C设备地址
 * @param  regAddr 要写入数据的寄存器地址
 * @param  bitNum  要写入数据的位
 * @param  data    写入的数据
 * @return        写入状态
 */
bool I2Cdev::writeBit(uint8_t devAddr,uint8_t regAddr,uint8_t bitNum,uint8_t data)
{
    uint8_t b;
    readByte(devAddr,regAddr,&b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return writeByte(devAddr,regAddr,b);
}
/**
 * 往一个16为寄存器写入一位
 * @param  devAddr I2C设备地址
 * @param  regAddr 要写入数据的寄存器地址
 * @param  bitNum  写入数据的位
 * @param  data    写入的数据
 * @return         写入状态
 */
bool I2Cdev::writeBitW(uint8_t devAddr,uint8_t regAddr,uint8_t bitNum,uint16_t data)
{
    uint16_t w;
    readWord(devAddr,regAddr,&w);
    w = (data != 0) ? (w | (1 << bitNum)) : (w & ~(1 << bitNum));
    return writeWord(devAddr,regAddr,w);
}
/**
 * 往一个8位寄存处写入多个bit
 * @param  devAddr  I2C设备地址
 * @param  regAddr  要写入数据的寄存器地址
 * @param  bitStart 开始位
 * @param  length   写入长度
 * @param  data     写入数据
 * @return          写入状态
 */
bool I2Cdev::writeBits(uint8_t devAddr,uint8_t regAddr,uint8_t bitStart,uint8_t length,uint8_t data)
{
    uint8_t b;
    if(readByte(devAddr,regAddr,&b) != 0)
    {
        uint8_t mask = (1 << length - 1) << bitStart;
        data &= mask;

        b &= ~(mask);
        b |= data;

        return writeByte(devAddr,regAddr,b);
    }
    else
    {
        return FALSE;
    }
}
/**
 * 往一个16位寄存器写入多个bit
 * @param  devAddr  I2C设备地址
 * @param  regAddr  要写入数据的寄存器地址
 * @param  bitStart 开始位
 * @param  length   写入长度
 * @param  data     写入数据
 * @return          写入状态
 */
bool I2Cdev::writeBitsW(uint8_t devAddr,uint8_t regAddr,uint8_t bitStart,uint8_t length,uint16_t data)
{
    uint16_t w;
    if(readWord(devAddr,regAddr,&w) != 0)
    {
        uint8_t mask = (1 << length - 1) << bitStart;
        data &= mask;
        w &= ~(mask);
        w |= data;

        return writeWord(devAddr,regAddr,w);
    }
    else
    {
        return FALSE;
    }
}
/**
 * 对8位寄存器写入一个Byte数据
 * @param  devAddr I2C设备地址
 * @param  regAddr 要写入数据的寄存器地址
 * @param  data    写入的数据
 * @return         写入状态
 */
bool I2Cdev::writeByte(uint8_t devAddr,uint8_t regAddr,uint8_t data)
{
    return writeBytes(devAddr,regAddr,1,&data);
}
/**
 * 往一个8位寄存器写入一个byte数据
 * @param  devAddr I2C设备地址
 * @param  regAddr 要写入数据的寄存器地址
 * @param  data    写入的数据
 * @return         写入状态
 */
bool I2Cdev::writeWord(uint8_t devAddr,uint8_t regAddr,uint16_t data)
{
    return writeBytes(devAddr,regAddr,1,&data);
}
/**
 * 往一个8位寄存器中写入多个字节数据
 * @param  devAddr I2C设备地址
 * @param  regAddr 要写入数据的寄存器地址
 * @param  length  写入数据的长度
 * @param  data    写入的数据
 * @return         写入状态
 */
bool I2Cdev::writeBytes(uint8_t devAddr,uint8_t regAddr,uint8_t length,uint8_t *data)
{
    int8_t count = 0;
    uint8_t buf[128];
    int fd;

    if(length > 127)
    {
        fprintf(stderr, "Byte write count (%d) >  127\n",length);
        return FALSE;
    }

    fd = open("/dev/i2c-1",O_RDWR);
    if(fd < 0)
    {
        fprintf(stderr, "Failed to open device:%s\n",strerror(errno));
        return FALSE;
    }

    if(ioctl(fd,I2C_SLAVE,devAddr) < 0)
    {
        fprintf(stderr, "Failed to select device:%s\n",strerror(errno));
        close(fd);
        return FALSE;
    }

    buf[0] = regAddr;
    memcpy(buf+1,data,length);
    count = write(fd,buf,length+1);
    if(count < 0)
    {
        fprintf(stderr, "Failed to write device(%d):%s\n",count,::strerror(errno));
        close(fd);
        return FALSE;
    }
    else if(count != length+1)
    {
        fprintf(stderr, "Short write to device,expected:%d, got:%d\n",length+1,count);
        close(fd);
        return FALSE;
    }

    close(fd);

    return TRUE;
}
/**
 * 往一个16位寄存器中写入多个word
 * @param  devAddr I2C设备地址
 * @param  regAddr 要写入数据的寄存器地址
 * @param  length  写入数据的长度
 * @param  data    写入的数据
 * @return         写入状态
 */
bool I2Cdev::writeWords(uint8_t devAddr,uint8_t regAddr,uint8_t length,uint16_t *data)
{
    int8_t count = 0;
    uint8_t buf[128];
    int i,fd;

    if(length > 63)
    {
        fprintf(stderr, "Word write count (%d) > 63\n",length);
        return FALSE;
    }

    fd = open("/dev/i2c-0",O_RDWR);
    if(fd < 0)
    {
        fprintf(stderr, "Failed to open device:%s\n",strerror(errno));
        return FALSE;
    }

    if(ioctl(fd,I2C_SLAVE,devAddr) < 0)
    {
        fprintf(stderr, "Failed to select device:%s\n",strerror(errno));
        close(fd);
        return FALSE;
    }

    buf[0] = regAddr;
    for(int i = 0;i < length; i++)
    {
        buf[i*2+1] = data[i] >> 8;
        buf[i*2+2] = data[i];
    }

    count = write(fd,buf,length*2+1);
    if(count < 0)
    {
        fprintf(stderr, "Failed to write device(%d):%s\n",count,::strerror(errno));
        close(fd);
        return FALSE;
    }
    else if(count != length*2+1)
    {
        fprintf(stderr, "Short write to device,expected:%d,got:%d\n",length*2+1,count);
        close(fd);
        return FALSE;
    }

    close(fd);
    return TRUE;
}
