#!/usr/bin/env bash

set -eo pipefail

WORKSPACE_DIR="${1:-.}"
OUTPUT_DIR="${WORKSPACE_DIR}/rpi3b/bin"
SDK_ENV="/opt/oecore-sdk/environment-setup-cortexa7t2hf-neon-vfpv4-oe-linux-gnueabi"

cd "${WORKSPACE_DIR}"

# Limpar ambiente host
unset LD_LIBRARY_PATH CC CXX LDFLAGS CFLAGS CXXFLAGS

# Ativar ROS2 host
source /opt/ros/jazzy/setup.bash

if [[ -f "install/setup.bash" ]]; then
  source install/setup.bash
fi

HOST_AMENT_PREFIX_PATH="${AMENT_PREFIX_PATH:-}"
HOST_CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-}"
unset LD_LIBRARY_PATH

if [[ ! -f "${SDK_ENV}" ]]; then
  echo "ERROR: SDK environment file not found at ${SDK_ENV}" >&2
  exit 1
fi

# Restaurar paths do host ANTES de ativar SDK
export PATH="/opt/ros/jazzy/bin:$PATH"
export LD_LIBRARY_PATH="/opt/ros/jazzy/lib:$LD_LIBRARY_PATH"

# Ativar SDK ARM (toolchain) - ignora warnings
source "${SDK_ENV}" 2>/dev/null || true

# Restaurar paths do host para encontrar ROS2 (após SDK)
export AMENT_PREFIX_PATH="${HOST_AMENT_PREFIX_PATH}:${AMENT_PREFIX_PATH:-}"
export CMAKE_PREFIX_PATH="${HOST_CMAKE_PREFIX_PATH}:${CMAKE_PREFIX_PATH:-}"
export PATH="/opt/ros/jazzy/bin:$PATH"

echo "Building micro-ROS Agent for Raspberry Pi 3 (ARMv7)"
echo "SDK: ${SDK_ENV}"
echo "CC: ${CC}"

rm -rf build/micro_ros_agent build/micro_ros_msgs \
  install/micro_ros_agent install/micro_ros_msgs

colcon build --packages-select micro_ros_agent \
  --parallel-workers $(nproc) \
  --cmake-args \
    -DCMAKE_C_COMPILER=/opt/oecore-sdk/sysroots/x86_64-oesdk-linux/usr/bin/arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-gcc \
    -DCMAKE_CXX_COMPILER=/opt/oecore-sdk/sysroots/x86_64-oesdk-linux/usr/bin/arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-g++ \
    -DCMAKE_C_FLAGS="-mthumb -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=/opt/oecore-sdk/sysroots/cortexa7t2hf-neon-vfpv4-oe-linux-gnueabi" \
    -DCMAKE_CXX_FLAGS="-mthumb -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=/opt/oecore-sdk/sysroots/cortexa7t2hf-neon-vfpv4-oe-linux-gnueabi" \
    -DUAGENT_BUILD_EXECUTABLE=ON \
    -DUAGENT_P2P_PROFILE=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_FORCE_C_COMPILER=ON \
    -DCMAKE_PREFIX_PATH="/opt/ros/jazzy:${WORKSPACE_DIR}/install" \
    -Dament_cmake_DIR=/opt/ros/jazzy/share/ament_cmake/cmake \
    -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH \
    -Dfastcdr_DIR=/opt/ros/jazzy/lib/cmake/fastcdr \
    --no-warn-unused-cli

AGENT_BINARY="$(find build install -type f -executable -name micro_ros_agent -print -quit)"
if [[ -z "${AGENT_BINARY}" ]]; then
  echo "ERROR: micro_ros_agent executable was not generated" >&2
  exit 1
fi

mkdir -p "${OUTPUT_DIR}"
cp "${AGENT_BINARY}" "${OUTPUT_DIR}/micro_ros_agent"
chmod +x "${OUTPUT_DIR}/micro_ros_agent"

echo "✅ Executable generated at: ${OUTPUT_DIR}/micro_ros_agent"
file "${OUTPUT_DIR}/micro_ros_agent"

if ! file "${OUTPUT_DIR}/micro_ros_agent" | grep -Eq 'ARM|arm'; then
  echo "❌ ERROR: generated executable is not an ARM binary" >&2
  exit 1
fi

echo "✅ ARMv7 binary confirmed!"
