#include "bsp.h"

#include "stm32g0xx.h"

#define EEPROM_PAGE_SIZE     2048U        // one page is 2KB
#define EEPROM_PAGE_ADDR     0x0800F800  // 最后一页 Page31
#define RECORD_SIZE          8           // 双字记录

typedef struct {
    uint16_t virtAddr;  // 虚拟地址
    uint32_t data;      // 数据
    uint8_t  bcc;       // 校验
    uint8_t  reserved;  // 对齐
} EEPROM_Record_t;

/* 解锁 Flash */
static void FLASH_Unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = 0x45670123U;
        FLASH->KEYR = 0xCDEF89ABU;
    }
}

/* 上锁 Flash */
static void FLASH_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

/* 等待空闲 */
static void FLASH_WaitBusy(void)
{
    while (FLASH->SR & FLASH_SR_BSY1) {}
}

/* 擦除最后一页 */
static void FLASH_ErasePage(void)
{
    FLASH_Unlock();
    FLASH_WaitBusy();

    FLASH->CR &= ~FLASH_CR_PNB;
    FLASH->CR |= FLASH_CR_PER;
    FLASH->CR |= (((EEPROM_PAGE_ADDR - 0x08000000) / EEPROM_PAGE_SIZE) << FLASH_CR_PNB_Pos);
    FLASH->CR |= FLASH_CR_STRT;

    FLASH_WaitBusy();
    FLASH->CR &= ~FLASH_CR_PER;

    FLASH_Lock();
}

/* 双字写入 */
void FLASH_ProgramDoubleWord(uint32_t addr, uint64_t data)
{
	   FLASH_Unlock();
	   FLASH_WaitBusy();
	
	   FLASH->CR |= FLASH_CR_PG;
	   *(volatile uint64_t*)addr = data;
	   FLASH_WaitBusy();
	   FLASH->CR &= ~FLASH_CR_PG;
	
	   FLASH_Lock();

}

/* 计算BCC */
static uint8_t EEPROM_CalcBCC(uint16_t virtAddr, uint32_t data)
{
    return (uint8_t)(virtAddr & 0xFF) ^
           (uint8_t)(virtAddr >> 8) ^
           (uint8_t)(data & 0xFF) ^
           (uint8_t)(data >> 8) ^
           (uint8_t)(data >> 16) ^
           (uint8_t)(data >> 24);
}

/* 读取虚拟地址对应的最新值 */
uint32_t EEPROM_Read(uint16_t virtAddr)
{
    uint32_t addr = EEPROM_PAGE_ADDR;
    uint32_t found = 0xFFFFFFFF;

    while (addr < EEPROM_PAGE_ADDR + EEPROM_PAGE_SIZE) {
        EEPROM_Record_t rec = *(EEPROM_Record_t*)addr;
        if (rec.virtAddr == 0xFFFF) break; // 空记录
        if (rec.virtAddr == virtAddr) {
            found = rec.data; // 最新值
        }
        addr += RECORD_SIZE;
    }
    return found;
}

/* 写入（追加模式 + 虚拟地址） */
void EEPROM_Write(uint16_t virtAddr, uint32_t data)
{
    uint32_t addr = EEPROM_PAGE_ADDR;

    // 找到空白位置
    while (addr < EEPROM_PAGE_ADDR + EEPROM_PAGE_SIZE) {
        EEPROM_Record_t rec = *(EEPROM_Record_t*)addr;
        if (rec.virtAddr == 0xFFFF) {
            EEPROM_Record_t newRec;
            newRec.virtAddr = virtAddr;
            newRec.data = data;
            newRec.bcc = EEPROM_CalcBCC(virtAddr, data);
            newRec.reserved = 0xFF; // 保留字节

            uint64_t packed;
            memcpy(&packed, &newRec, sizeof(newRec));
            FLASH_ProgramDoubleWord(addr, packed);
            return;
        }
        addr += RECORD_SIZE;
    }

    // 页满 → 擦除并重写最新值
    // 这里可以遍历所有虚拟地址并重写最新值
   //  FLASH_ErasePage();
   // EEPROM_Write(virtAddr, data);
}

#if 0
void read_flash_eeprom_value(void)
{
    uint32_t val1, val2;

    FLASH_ErasePage(); // 首次清空

    EEPROM_Write(0x0001, 0x12345678); // 写入参数1
    EEPROM_Write(0x0002, 0x9ABCDEF0); // 写入参数2

    // 更新参数1
    EEPROM_Write(0x0001, 0x11111111);

    val1 = EEPROM_Read(0x0001);
    val2 = EEPROM_Read(0x0002);

}
#endif

uint32_t read_flash_value(void)
{
	

	return EEPROM_Read(0x0001);

}
