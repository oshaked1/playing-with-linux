# Kprobe Exec Monitor

This simple kernel module attaches a kprobe to the `do_execveat_common` kernel function (which handles all `exec` type requests).

The kprobe prints some information about the request, including the requesting PID, the file to execute, and up to 4 arguments.

:warning: This is just a quick hack and should definitely not be used in a production environment!

# How to build

This works on 64-bit systems only.

First, make sure you have dependencies installed.

On Ubuntu:

```bash
sudo apt install build-essential make linux-headers-$(uname -r)
```

To build, run `make`. To clean, run `make clean`.

# How to run

Load the kernel module:

```
sudo insmod exec_monitor.ko
```

To read the output (or errors if it doesn't work):

```bash
# read all output so far
dmesg

# read output continuously
dmesg -wH
```

Unload the kernel module:

```bash
sudo rmmod exec_monitor
```

