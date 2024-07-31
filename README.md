
## test 
build\efi\bootにBOOTX64.EFIを設置し、
```bash
qemu-system-x86_64.exe -bios {OVMF.fdまでのパス} -hda fat:rw:build
```
