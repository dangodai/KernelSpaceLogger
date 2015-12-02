#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_AUTHOR("Reilly Moore, Brandon Cryderman");
MODULE_LICENSE("GPL");

static int __init init_keylogger(void){
	return 0;
}

static void __exit exit_keylogger(void){

}

module_init(init_keylogger);
module_exit(exit_keylogger);