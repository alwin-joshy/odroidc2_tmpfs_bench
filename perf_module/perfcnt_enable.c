#include <linux/module.h>
#include <linux/kernel.h>
 
static void enable_ccnt(void *data) {
  // Set the User Enable register, bit 0
  asm volatile ("msr PMUSERENR_EL0, %0" :: "r"(1));
  
  // Disable overflow interrupts on all counters
  asm volatile ("msr PMCNTENCLR_EL0, %0" :: "r"(-1));

  printk("Enabling user level cycle counter on core\n");
}

int init_module(void) {
  on_each_cpu(enable_ccnt, NULL, 1); 
  printk("User level cycle counter module loaded\n");
  return 0;
}
 
void cleanup_module(void) {
}

MODULE_LICENSE ("GPL v2");
