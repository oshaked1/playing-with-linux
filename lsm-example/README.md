# LSM Example

This is an example of a minimal LSM (Linux Security Module).

All it does is place a hook on the security check for creating a directory (`security_path_mkdir`) and logs every attempt.

Additionally, if a directory named "forbidden" is created, the action is blocked.

## How to Use

As LSMs cannot be in the form of a kernel module, direct modification of the kernel source code is required.

To compile and run a minimal kernel, follow [this](https://www.zachpfeffer.com/single-post/build-the-linux-kernel-and-busybox-and-run-on-qemu) excellent guide.

To use the LSM, add the source files provided here to the kernel source using the same directory structure as they are organized here (replace files which already exist).

:warning: This LSM was written for Linux 4.10.6. Replacing the source files of a different kernel version with the files provided here will likely break stuff.

Once the source files are added, the kernel config needs to be updated (using the same menu config as used in the linked guide).

The extra options that need to be activated:

```
General setup  --->
	Configure standard kernel features (expert users)  --->
		[*]   Multiple users, groups and capabilities support

Security options  --->
	[*] Enable different security models
	[*] Security hooks for pathname based access control
	[*] Example LSM
```

Once they are activated, run `make mrproper` and then compile the kernel normally and run it with QEMU.

The LSM is activated when the kernel initializes, and will work immediately after boot.

## Example

```
/ # mkdir /root/test
Example LSM: mkdir (19) created directory /root/test
/ # mkdir /root/test/forbidden
Example LSM: blocked attempt to create directory "forbidden" under /root/test
mkdir: can't create directory '/root/test/forbidden': Permission denied
```

The messages printed by the LSM go to dmsg, but the example above was made in a console session so all kernel messages are displayed.