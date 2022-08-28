# What

**dump-kcore.py** is a python script that dumps the physical memory of a Linux system using the `/proc/kcore` virtual file.

:warning: **Please do not use this in a production environment, it is barely tested** (and only supports 64-bit systems).

# Why

For fun and profit.

# How

In principal, the physical memory regions that are mapped to RAM are enumerated from `/proc/iomem`,  and these ranges are extracted from the identity-mapped physical memory regions in the kernel's virtual address space which is accessible using the `/proc/kcore` file.

Was that a load of nonsense? [This article](https://schlafwandler.github.io/posts/dumping-/proc/kcore/) does a much better job explaining it.

# Which

**dump-kcore.py** uses the [LiME ](https://github.com/504ensicsLabs/LiME)format for output. This output format is supported by the [Volatility Framework](https://www.volatilityfoundation.org/).

# Whoa...
**dump-kcore.py** is seriously fast. In fact, it is faster than the popular usermode memory-acquisition tool AVML, which also uses the same dumping method.
The performance of this script stems from the usage of the `copy_file_range` system call, which performs all of the data moving within the kernel instead of copying to userspace and back into the kernel.
For reference, AVML uses a simple (buffered) read and write solution, which is not only slower, but has a larger impact on the state of the acquired memory.

## Some numbers
On my test machine (Ubuntu Server 20.04.01 LTS running kernel 5.4.0-100-generic) with 4GB of RAM,
AVML takes between 23-28 seconds to perform a memory dump, while **dump-kcore.py** takes 17-19 seconds!