#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/notifier.h>
#include <linux/syscalls.h> 
#include <linux/file.h> 
#include <linux/fs.h>
#include <linux/fcntl.h> 
#include <asm/uaccess.h> 
#include <linux/mm.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

#include<linux/types.h>

#include<linux/fs.h>
#include<linux/string.h>
#include<asm/uaccess.h>

#define FILE_DIR "/root/test.txt"


char *buff = "module read/write test";
char tmp[100];
static struct file *filp = NULL;

void log_keys(char[16] buff){
  mm_segment_t old_fs;
  filp = filp_open(FILE_DIR, O_RDWR | O_CREAT, 0644);
  //    if(!filp)
  if(IS_ERR(filp))
  printk("open error...\n");
  old_fs = get_fs();
  set_fs(get_ds());
  filp->f_op->write(filp, buff, strlen(buff), &filp->f_pos);
  set_fs(old_fs);
  if(filp)
    filp_close(filp,NULL);
}




