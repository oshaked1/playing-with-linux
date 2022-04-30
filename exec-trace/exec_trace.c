#include <linux/module.h>
#include <linux/tracepoint.h>
#include <linux/binfmts.h>

char *tp_name = "sched_process_exec";
struct tracepoint *tp;

static void probe_sched_process_exec(void *data, struct task_struct *p,
                                    pid_t old_pid, struct linux_binprm *bprm)
{
    printk("exec by pid %d: filename=%s", p->pid, bprm->filename);
}

static void find_tracepoint(struct tracepoint *curr_tp, void *ignore) {
    if (strcmp(tp_name, curr_tp->name) == 0)
        tp = curr_tp;
}

static int __init trace_init(void)
{
    // find the requested tracepoint
    for_each_kernel_tracepoint(find_tracepoint, NULL);

    if (tp == NULL) {
        printk("Error, %s not found\n", tp_name);
        return -EINVAL;
    }

    tracepoint_probe_register(tp, probe_sched_process_exec, NULL);
    return 0;
}

static void __exit trace_exit(void)
{
    tracepoint_probe_unregister(tp, probe_sched_process_exec, NULL);
}

module_init(trace_init)
module_exit(trace_exit)
MODULE_LICENSE("GPL");