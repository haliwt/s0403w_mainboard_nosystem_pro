#include "bsp.h"

typedef struct
{
    uint8_t header;      // 0xA5
    uint8_t dev_addr;    // 0x01 / 0x02
    uint8_t ack_type;    // 0x80 (新版) 或 0xFF (旧版)
    uint8_t cmd_type;    // 复制原命令
    uint8_t func_code;   // 复制原功能码
    uint8_t data_len;    // 数据长度
    uint8_t data[32];    // 数据区（一般不用）
    uint8_t tail;        // 0xFE
    uint8_t bcc;         // 校验
} S03AckFrame_t;


uint8_t S03_CalcBCC(const uint8_t *buf, uint16_t len)
{
    uint8_t bcc = 0;
    for(uint16_t i = 0; i < len; i++)
        bcc ^= buf[i];
    return bcc;
}


void S03_SendAck(uint8_t cmd_type, uint8_t func_code)
{
    uint8_t buf[16];
    uint8_t index = 0;
	uint8_t bcc;

    // 帧头
    buf[index++] = 0x5A ;// mainboard //0xA5;        // 显示板帧头

    // 设备地址
    buf[index++] = 0x10;    // 0x01 内显 / 0x02 外显

    // 应答类型（新版协议）
    buf[index++] = 0xFF; //       // 新版应答成功 0x80

    // 复制命令类型
    buf[index++] = cmd_type;

    // 复制功能码
    buf[index++] = func_code;

    // 数据长度（一般为 0）
    buf[index++] = 0x00;

    // 帧尾
    buf[index++] = 0xFE;

    // BCC 校验（从帧头到帧尾）
    bcc = S03_CalcBCC(buf, index);
    buf[index++] = bcc;

    // 发送
    //UART1_SendBytes(buf, index);
    usart1_dma_send(buf,index);
}

