# bluemoon
> Hobby operating system

### Getting Started
#### Install Toolchain
We will need the following toolchain setup:
- [nasm](https://www.nasm.us/)
- [binutils](https://www.gnu.org/software/binutils/)
- [gcc](https://www.gnu.org/software/gcc/)
- [gdb](https://www.gnu.org/software/gdb/)
- [qemu](https://www.qemu.org/)

To save all the trouble, you can run the auto installer:
```
toolchain/setup.sh
```

#### Prepare Test Image
To run bluemoon on emulator, we need to create a virtual disk image.
```
make image
```
This will create a 10MB empty image on `dist/bluemoon.img`  
Next, install the boot loader:
```
make boot
```
Launch emulator:
```
make run-i686
make run-x86_64
```
