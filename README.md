# EDI_tests

Fork of [edi-demo](https://github.com/Novakov/qemu-edi-demo) Repository.

# Build

1. Generate CMake build: `cmake -S . -B build`
2. Build project (`cmake --build <build-dir>` or `make`/`ninja` in build directory)

# QEMU

1. Create venv `<build>/venv`
2. Install requirements `<build>/venv/Scripts/pip install -r requirements.txt`
3. Start listener `<build>/venv/Scripts/python <src>/host/listener.py`
4. Start
   QEMU: `qemu-system-arm -machine virt_cortex_m -semihosting -semihosting-config enable=on,target=native -monitor null -kernel <build>/app/qemu_runner/app_qemu.elf -device kp-edi-group`
5. Listener will receive logs from firmware and simulate button press every few seconds
