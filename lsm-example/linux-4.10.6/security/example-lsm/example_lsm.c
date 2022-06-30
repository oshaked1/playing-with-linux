#include <linux/lsm_hooks.h>
#include <linux/kernel.h>
#include <linux/fs_struct.h>

int path_mkdir_hook(const struct path *dir, struct dentry *dentry, umode_t mode)
{
    char *pathstr_orig = kmalloc(256, GFP_KERNEL);
    char *pathstr;
    if (pathstr_orig == NULL) {
        pr_err("Example LSM: kmalloc error\n");
        return 0;
    }
    int err;
    if (!strcmp(dentry->d_name.name, "forbidden")) {
        pathstr = d_path(dir, pathstr_orig, 256);
        pr_info("Example LSM: blocked attempt to create directory \"forbidden\" under %s\n", pathstr);
        err = -EACCES;
        goto out;
    }
    else {
        pathstr = dentry_path(dentry, pathstr_orig, 256);
        pr_info("Example LSM: %s (%d) created directory %s\n", current->comm, current->pid, pathstr);
        err = 0;
        goto out;
    }
out:
    kfree(pathstr_orig);
    return err;
}

static struct security_hook_list example_lsm_hooks[] = {
	LSM_HOOK_INIT(path_mkdir, path_mkdir_hook)
};

void __init example_lsm_add_hooks(void)
{
	pr_notice("Example LSM: installing hooks\n");
	security_add_hooks(example_lsm_hooks, ARRAY_SIZE(example_lsm_hooks));
}