#ifndef __MEMORY_H__
#define __MEMORY_H__


// static inline bool in_pmem(paddr_t addr) {
//   return (addr >= CONFIG_MBASE) && (addr - CONFIG_MSIZE < (paddr_t)CONFIG_MBASE);
// }

 u_int64_t paddr_read(u_int64_t addr, int len);
void paddr_write(u_int64_t addr, int len, u_int64_t data);

#endif