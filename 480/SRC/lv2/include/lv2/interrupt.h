#ifndef __LV2_INTERRUPT_H__
#define __LV2_INTERRUPT_H__

#include <lv2/lv2.h>

extern void suspend_intr(void) __asm__("suspend_intr");
extern void resume_intr(void) __asm__("resume_intr");

LV2_EXPORT uint64_t spin_lock_irqsave_ex(void *ta);
LV2_EXPORT void spin_unlock_irqrestore_ex(void *ta, uint64_t restore, int zero);

uint64_t spin_lock_irqsave(void);
void spin_unlock_irqrestore(uint64_t restore);

#endif /* __LV2_INTERRUPT_H__ */

