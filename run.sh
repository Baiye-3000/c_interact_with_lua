#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT}/build"

echo "==> 项目目录: ${ROOT}"
echo "==> 配置并编译..."
cmake -S "${ROOT}" -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}"

echo ""
echo "==> 运行程序..."
cd "${ROOT}"
"${BUILD_DIR}/main"
