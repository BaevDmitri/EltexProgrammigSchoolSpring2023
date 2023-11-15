#include <linux/module.h> /* Библиотека модулей */ 
#include <linux/printk.h> /* pr_info() */ 

int init_module(void)
{
    pr_info("Module is loaded.\n");
    return 0;
} 

void cleanup_module(void) 
{ 
    pr_info("Module is unloaded.\n"); 
} 

MODULE_LICENSE("GPL");
