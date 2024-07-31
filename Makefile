BOOTX64.EFI: main.c efi.c common.c
	x86_64-w64-mingw32-gcc -Wall -Wextra -e efi_main -nostdinc -nostdlib -fno-builtin -Wl,--subsystem,10 -I. -o $@ $+

clean:
	rm -f BOOTX64.EFI

.PHONY: clean