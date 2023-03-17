#ifndef __NPC_H__
#define __NPC_H__


#define DEVICE_BASE 0xa0000000

#define SERIAL_PORT     (DEVICE_BASE + 0x00003f8)
#define RTC_ADDR        (DEVICE_BASE + 0x0000048)
#define VGACTL_ADDR     (DEVICE_BASE + 0x0000100)
#define FB_ADDR         (DEVICE_BASE + 0x1000000)

#define SYNC_ADDR       (VGACTL_ADDR + 4)

#endif