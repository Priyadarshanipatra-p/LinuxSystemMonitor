#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>
#include <linux/sysinfo.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Priyadarshani Patra");
MODULE_DESCRIPTION("Linux System Monitor kernel module");
MODULE_VERSION("1.0");

#define PROC_NAME "resource_monitor"

static struct proc_dir_entry *proc_entry;

static int resource_monitor_show(struct seq_file *m, void *v)
{
    struct sysinfo info;
    unsigned long total_ram;
    unsigned long free_ram;

    si_meminfo(&info);

    total_ram = info.totalram * (info.mem_unit / 1024);
    free_ram = info.freeram * (info.mem_unit / 1024);

    seq_printf(m, "Linux System Monitor Kernel Module\n");
    seq_printf(m, "=================================\n");
    seq_printf(m, "Total RAM: %lu KB\n", total_ram);
    seq_printf(m, "Free RAM: %lu KB\n", free_ram);

    return 0;
}

static int resource_monitor_open(struct inode *inode, struct file *file)
{
    return single_open(file, resource_monitor_show, NULL);
}

static const struct proc_ops resource_monitor_proc_ops = {
    .proc_open = resource_monitor_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init resource_monitor_init(void)
{
    proc_entry = proc_create(PROC_NAME, 0444, NULL,
                             &resource_monitor_proc_ops);

    if (!proc_entry) {
        pr_err("Resource Monitor: failed to create /proc/%s\n",
               PROC_NAME);
        return -ENOMEM;
    }

    pr_info("Resource Monitor: kernel module loaded\n");
    pr_info("Resource Monitor: /proc/%s created\n", PROC_NAME);

    return 0;
}

static void __exit resource_monitor_exit(void)
{
    proc_remove(proc_entry);

    pr_info("Resource Monitor: kernel module unloaded\n");
}

module_init(resource_monitor_init);
module_exit(resource_monitor_exit);
