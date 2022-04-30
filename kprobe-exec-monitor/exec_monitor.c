#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>

#define MAX_SYMBOL_LEN	64
#define MAX_ARGS 4
static char symbol[MAX_SYMBOL_LEN] = "do_execveat_common.isra.0";

static struct kprobe kp = {
	.symbol_name	= symbol,
};

int count_args(char **argv) {
	char *arg;
	int i = 0;
	do {
		get_user(arg, argv + i);
		i++;
	}
	while (arg != NULL);
	return i - 1;
}

void copy_arg(char *dst, char **argv, int idx) {
	char *user_arg;
	get_user(user_arg, argv + idx);
	strncpy_from_user(dst, user_arg, 100);
}

void get_args(char **kernel_argv, char **user_argv, int argc) {
	int i;
	for (i = 0; i < MAX_ARGS; i++) {
		if (i < argc) {
			kernel_argv[i] = kvmalloc(100, GFP_KERNEL);
			copy_arg(kernel_argv[i], user_argv, i);
		}
		else
			kernel_argv[i] = "NULL";
	}
}

static int __kprobes handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	const char *filename = ((struct filename*)regs->si)->name;
	int argc;
	char **user_argv;
	char *argv[MAX_ARGS];

#ifdef CONFIG_COMPAT
	user_argv = (char**)regs->cx;
#else
	user_argv = (char**)regs->dx;
#endif

	argc = count_args(user_argv);
	get_args(argv, user_argv, argc);

	printk("exec by pid %d: filename=%s argc=%d argv[0]=%s argv[1]=%s argv[2]=%s argv[3]=%s",
		current->pid, filename, argc, argv[0], argv[1], argv[2], argv[3]);
    return 0;
}

static int __init kprobe_init(void)
{
	int ret;
	kp.pre_handler = handler_pre;

	ret = register_kprobe(&kp);
	if (ret < 0) {
		printk("register_kprobe failed, returned %d\n", ret);
		return ret;
	}
	printk("Planted kprobe at %p\n", kp.addr);
	return 0;
}

static void __exit kprobe_exit(void)
{
	unregister_kprobe(&kp);
	printk("kprobe at %p unregistered\n", kp.addr);
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");