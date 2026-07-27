#!/usr/bin/env bash
# Build (if needed) and drop into the ARM-M3 build container with this repo
# mounted at /workspace, for local compilation of the PSoC 5LP firmware.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE_NAME="rc-car-arm-m3"

docker build -t "${IMAGE_NAME}" "${REPO_ROOT}"

docker run --rm -it \
    -v "${REPO_ROOT}:/workspace" \
    -w /workspace \
    --device=/dev/bus/usb:/dev/bus/usb \
    --device-cgroup-rule="c 189:* rmw" \
    --cap-add=SYS_ADMIN \
    "${IMAGE_NAME}" \
    "$@"
