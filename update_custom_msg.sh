#!/usr/bin/env bash
#
# Atualiza as mensagens customizadas e regenera o firmware do Pico.
#
#   ./update_custom_msg.sh          rotina: msgs + firmware (incremental)
#   ./update_custom_msg.sh --full   setup: limpa tudo e reconstroi o agent x86
#
# O agent x86 da VM serve para testar localmente (Pico ligado na VM). Ele NAO
# precisa ser reconstruido a cada alteracao de mensagem -- so no setup inicial
# ou quando a versao do micro-ROS mudar. Por isso ficou atras do --full.
# O agent do Raspberry e outro: vem pronto do Yocto (MicroXRCEAgent, ARM).

set -Eeuo pipefail

FULL="${1:-}"

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
LOG_DIR="${SCRIPT_DIR}/.logs"
mkdir -p "$LOG_DIR"
LOG_FILE="${LOG_DIR}/update_custom_msg_$(date +%Y%m%d_%H%M%S).log"

log() {
    printf '[%s] %s\n' "$(date '+%Y-%m-%d %H:%M:%S')" "$*"
}

exec > >(tee -a "$LOG_FILE") 2>&1

trap 'status=$?; log "ERRO na linha ${LINENO}: ${BASH_COMMAND} (status ${status})"; exit "$status"' ERR

cd "$SCRIPT_DIR"

log "Iniciando atualizacao de mensagens${FULL:+ (modo $FULL)}"
log "Log salvo em: ${LOG_FILE}"

# Ambiente limpo e previsivel: o ~/.bashrc pode ter sourceado um overlay cujo
# install/ este script apaga, o que gera os warnings "path doesn't exist".
log "Carregando ambiente do ROS"
unset AMENT_PREFIX_PATH CMAKE_PREFIX_PATH COLCON_PREFIX_PATH AMENT_CURRENT_PREFIX
set +u; source /opt/ros/jazzy/setup.bash; set -u

if [[ "$FULL" == "--full" ]]; then
    log "Limpando install, build e log (build do zero)"
    sudo rm -rf install build log firmware firmware src/uros
else
    log "Build incremental (use --full para limpar o cache)"
fi

# O colcon paraleliza PACOTES, mas o make/ninja dentro de cada um roda serial
# por padrao (-j1). Num superbuild grande e sozinho isso deixa 8 dos 9 cores
# ociosos. MAKEFLAGS e herdado tambem pelo ExternalProject do superbuild.
export MAKEFLAGS="-j$(nproc)"
log "Paralelismo: MAKEFLAGS=${MAKEFLAGS}"

log "Compilando workspace"
colcon build

log "Carregando install/setup.bash"
set +u; source install/setup.bash; set -u

if [[ "$FULL" == "--full" ]]; then
    log "Criando workspace do agente micro-ROS (x86, para testes na VM)"
    ros2 run micro_ros_setup create_agent_ws.sh

    log "Compilando agente micro-ROS"
    ros2 run micro_ros_setup build_agent.sh
else
    log "Agente micro-ROS: mantido (use --full para reconstruir)"
fi

if [[ "$FULL" == "--full" ]]; then
    log "Criando firmware workspace"
    ros2 run micro_ros_setup create_firmware_ws.sh generate_lib
else
    log "Firmware workspace: mantido (use --full para recriar)"
fi

log "Criando workspace de firmware das interfaces"
ros2 run robot_interfaces create_fwws.sh

log "Compilando firmware"
ros2 run micro_ros_setup build_firmware.sh \
    "${SCRIPT_DIR}/src/pico_toolchain.cmake" \
    "${SCRIPT_DIR}/src/pico_colcon.meta"

log "Atualizacao concluida com sucesso"
