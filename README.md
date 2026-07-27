# RC-Car-peripheral-controller
The following repository implements the peripheral controller for the autonomous RC car, based around the PSoC 5LP.

# Install ARM-M3 Toolchain

To develop and compile code for the PSoC 5LP, you need to install the ARM-M3 toolchain. Follow the steps below:

1. Download the ARM-M3 toolchain from the official ARM website: [ARM Developer](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
2. Follow the installation instructions for your operating system.
3. Verify the installation by running the following command in your terminal:
   ```bash
   arm-none-eabi-gcc --version
   ```

## Linux: Docker Dev Environment

Alternatively, on Linux you can use the provided `Dockerfile` to get a container with the ARM-M3 toolchain preinstalled, without touching your host system.

### Build the image

```bash
docker compose build
```

or, without Docker Compose:

```bash
docker build -t rc-car-arm-m3 .
```

### Start a dev shell

```bash
docker compose run --rm arm-m3-dev
```

or, without Docker Compose:

```bash
./scripts/docker-dev.sh
```

Either builds the `rc-car-arm-m3` image first if it doesn't exist yet, then drops you into a shell inside the container, with the repository mounted at `/workspace`.

# Building the Firmware

Make sure the FreeRTOS submodule is checked out first:

```bash
git submodule update --init --recursive
```

Then, from inside the Docker dev shell (or any environment with `arm-none-eabi-gcc` and `cmake` on `PATH`), configure and build with CMake:

```bash
cmake -B build
cmake --build build
```

This produces `build/rc-car-peripheral-controller.elf`, `.hex`, `.bin`, and `.map`.