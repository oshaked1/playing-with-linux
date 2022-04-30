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
