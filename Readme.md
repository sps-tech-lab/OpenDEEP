
# OpenDEEP

![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/sps-tech-lab/OpenDEEP?label=version)
![License](https://img.shields.io/badge/license-GPLv3-green.svg?style=flat)

A battery-powered **free-diving depth gauge prototype** built end-to-end around **MS5837** pressure sensing.
Current prototype targets **0–10 m** depth range and serves as a base for expanding features and power optimization.

![Appearance](./docs/img/open_deep_poster.png)

#### Hardware
- MCU: **RP2040**
- Display: **IPS 0.99 inch**
- Pressure / temperature sensor: **MS5837-02BA**

#### Roadmap
- Ultra-low-power variant on **PR2350 / nRF52832**
- Zephyr OS port
- Accelerometer
- Magnetic field sensor 

---

### Getting Started
After cloning the repo, run:

```
python3 scripts/generate_presets.py --generator Ninja
```
This script will produce CMakePresets.json from CMakePresets.json.def using your generator

```Ninja / "Unix Makefiles" / "Visual Studio 17 2022" / "MinGW Makefiles"```

#### CLion
In CLion go: 
```Settings``` → ```Build, Execution, Deployment``` → ```CMake```
and choose one of profiles. If there is no one that satisfy you, 
create a new in CMakePresets.json.def and run ```generate_presets.py ```again

#### Command Line
For command line it would be:
```
cmake --preset <YourBoardPreset>
cmake --build --preset <YourBoardPreset>
```

After it, edit `CMakeLists.txt` in `<root_folder>`:
```cmake
# rename
project(pico-cmake-project)
# to
project(your_project_name)
```

---

## Dependencies

For this project [pico-sdk](https://github.com/raspberrypi/pico-sdk) is supposed to be placed in the same directory as the ```project folder``` placed

---

### Compiler

In the latest Pico-SDK workflow the SDK does not bundle its own compiler—you’re expected to install and manage 
the ARM GCC toolchain yourself.

#### MacOS

Install the ARM Embedded GCC toolchain
```shell
# Homebrew
brew tap ArmMbed/homebrew-formulae
brew install arm-none-eabi-gcc cmake

# Ports
sudo port selfupdate
sudo port install arm-none-eabi-gcc arm-none-eabi-binutils
```
By default MacPorts binaries live in /opt/local/bin, so in your shell init (e.g. ~/.zshrc) you should have:
```export PATH=/opt/local/bin:/opt/local/sbin:$PATH```

Once arm-none-eabi-gcc is on your PATH and PICO_SDK_PATH is set, the Pico-SDK’s CMake logic will automatically pick
the correct pico_arm_cortex_m0plus_gcc.cmake toolchain file, and your macOS build should succeed just like on Windows.



#### Windows

1. **Install the ARM GNU Toolchain**

    - **Chocolatey** (recommended because it also gives you `cmake` and `ninja` in one shot):

      ```powershell
      choco install gcc-arm-embedded cmake ninja
      ```

    - **Manual installer** (if you don’t use Chocolatey):

        1. Download the latest *“Arm GNU Toolchain for Windows”* ZIP from  
           <https://developer.arm.com/downloads/-/gnu-rm>.
        2. Extract to `C:\ArmGCC`.
        3. Add the `bin` folder to your *System* `PATH` (e.g. `C:\ArmGCC\bin`).

2. **Verify the toolchain is visible**

   ```powershell
   arm-none-eabi-gcc --version
   ```
   You should see the version banner instead of “command not found.”