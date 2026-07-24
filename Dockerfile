# Build/dev environment for the PSoC 5LP (CY8C5888LTI-LP097, Cortex-M3) firmware.
# Provides the ARM GNU toolchain referenced in README.md plus the host build tools
# needed to compile the project generated sources on Linux.
FROM ubuntu:22.04

ARG ARM_TOOLCHAIN_VERSION=13.3.rel1
ARG ARM_TOOLCHAIN_ARCHIVE=arm-gnu-toolchain-${ARM_TOOLCHAIN_VERSION}-x86_64-arm-none-eabi.tar.xz
ARG ARM_TOOLCHAIN_URL=https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_TOOLCHAIN_VERSION}/binrel/${ARM_TOOLCHAIN_ARCHIVE}
ARG ARM_TOOLCHAIN_PREFIX=/opt/arm-gnu-toolchain

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        git \
        make \
        wget \
        xz-utils \
        ca-certificates \
        libncurses6 \
        libncursesw6 \
        libpython3.10 \
        python3 \
        net-tools \
        iproute2 \
    && rm -rf /var/lib/apt/lists/*

RUN wget -qO /tmp/arm-toolchain.tar.xz "${ARM_TOOLCHAIN_URL}" \
    && mkdir -p "${ARM_TOOLCHAIN_PREFIX}" \
    && tar -xJf /tmp/arm-toolchain.tar.xz -C "${ARM_TOOLCHAIN_PREFIX}" --strip-components=1 \
    && rm /tmp/arm-toolchain.tar.xz

ENV PATH="${ARM_TOOLCHAIN_PREFIX}/bin:${PATH}"

RUN arm-none-eabi-gcc --version

WORKDIR /workspace

CMD ["/bin/bash"]
