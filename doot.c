#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/random.h>
#include <linux/kallsyms.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Taylor");
MODULE_DESCRIPTION("Do not load this module!!!1111ONE!1!111ONE!!!ELEVEN!! It is 2 spoopy!!!ONE11!!");

/* open syscall with distinct lack of doots */
asmlinkage long (*no_doot_openat)(int dfd, const char __user *, int, umode_t);
unsigned long *syscall_table;

#define SHARE "/usr/local/share/skeltal/"
static char *doot_png = SHARE "doot.png";
static char *doot_svg = SHARE "doot.svg";
static char *doot_jpg = SHARE "doot.jpg";
static char *doot_gif = SHARE "doot_black.gif";

static long doots;


static void remove_wp(void)
{
    write_cr0(read_cr0() & (~ 0x10000));
}


static void enable_wp(void)
{
    write_cr0(read_cr0() | 0x10000);
}


asmlinkage long get_fd(int dfd, const char *name, int flags, umode_t mode)
{
    mm_segment_t old_fs;
    long fd;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    fd = (*no_doot_openat)(dfd, name, flags, mode);
    set_fs(old_fs);

    return fd;
}


static int filecmp(const char *filename, const char *ext)
{
    int l = strlen(filename);
    return !strcmp(filename + l - 4, ext);
}


static bool to_doot_or_not_to_doot(void) {
    // Always dooting.
    return true;
}


#define LOG_DOOT if (printk_ratelimit()) \
    printk(KERN_INFO "dooting '%s'!\n", filename)

asmlinkage long doot_open(int dfd, const char __user *filename, int flags, umode_t mode)
{
    if (to_doot_or_not_to_doot()) {
        if (filecmp(filename, ".png") || filecmp(filename, ".PNG")) {
            LOG_DOOT;
            return get_fd(dfd, doot_png, flags, mode);
        } else if (filecmp(filename, ".svg") || filecmp(filename, ".SVG")) {
            LOG_DOOT;
            return get_fd(dfd, doot_svg, flags, mode);
        } else if (filecmp(filename, ".jpg") || filecmp(filename, ".JPG")) {
            LOG_DOOT;
            return get_fd(dfd, doot_jpg, flags, mode);
        } else if (filecmp(filename, ".gif") || filecmp(filename, ".GIF")) {
            LOG_DOOT;
            return get_fd(dfd, doot_gif, flags, mode);
        }
    }
    return (*no_doot_openat)(dfd, filename, flags, mode);
}


/* static unsigned long **find_call_table(void) */
/* { */
/*     unsigned long o; */
/*     unsigned long **call_table_ptr; */

/*     for (o = PAGE_OFFSET; o < ULLONG_MAX; o += sizeof(void *)) { */
/*         call_table_ptr = (unsigned long **) o; */
/*         if (call_table_ptr[__NR_close] == (unsigned long *) sys_close) { */
/*             return call_table_ptr; */
/*         } */
/*     } */

/*     return NULL; */
/* } */


static int __init doot_init(void)
{
    doots = 0;

    syscall_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

    remove_wp();
    no_doot_openat = (void *) syscall_table[__NR_openat];
    syscall_table[__NR_openat] = (unsigned long) doot_open;
    enable_wp();

    printk(KERN_INFO "oh no! Mr Skeltal is loose inside ur computer!\n");

    return 0;
}


static void __exit doot_cleanup(void)
{
    printk(KERN_INFO "dooted our last doot rip in piece\n");

    remove_wp();
    syscall_table[__NR_openat] = (unsigned long) no_doot_openat;
    enable_wp();
}


module_init(doot_init);
module_exit(doot_cleanup);
