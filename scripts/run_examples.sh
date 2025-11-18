#!/usr/bin/env bash
# Build (and optionally execute) every example target to ensure documentation samples stay fresh.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
GENERATOR_ARGS=${GENERATOR_ARGS:-""}
LOG_FILE="${RUN_EXAMPLES_LOG:-${ROOT_DIR}/build/example_builds.log}"
JOBS_FLAG="${JOBS:-""}"

mkdir -p "$(dirname "$LOG_FILE")"

usage() {
    cat <<'EOF'
Usage: scripts/run_examples.sh [options]

Options:
  -x, --execute           Run each example binary after building it
  -f, --filter <pattern>  Only process example directories matching pattern (glob)
  -l, --list              List detected examples and exit
  -j, --jobs <N>          Pass --parallel N to cmake --build (or set JOBS env var)
  -h, --help              Show this message

Environment variables:
  BUILD_DIR         Override the build directory (default: ./build)
  GENERATOR_ARGS    Extra flags passed to the initial cmake configure step
  RUN_EXAMPLES_LOG  Log file path (default: build/example_builds.log)
  JOBS              Parallelism for cmake --build (same as --jobs)
EOF
}

EXECUTE=false
LIST_ONLY=false
FILTER_PATTERN=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -x|--execute)
            EXECUTE=true
            shift
            ;;
        -f|--filter)
            FILTER_PATTERN=${2:-}
            [[ -z "$FILTER_PATTERN" ]] && { echo "Missing value for --filter" >&2; exit 1; }
            shift 2
            ;;
        -l|--list)
            LIST_ONLY=true
            shift
            ;;
        -j|--jobs)
            JOBS_FLAG=${2:-}
            [[ -z "$JOBS_FLAG" ]] && { echo "Missing value for --jobs" >&2; exit 1; }
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1" >&2
            usage
            exit 1
            ;;
    esac
done

configure_if_needed() {
    if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
        echo "[run_examples] Configuring CMake in ${BUILD_DIR}" | tee -a "$LOG_FILE"
        cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DBUILD_EXAMPLES=ON $GENERATOR_ARGS
    fi
}

build_example() {
    local name="$1"
    local target="example_${name}"
    local parallel_args=()
    if [[ -n "$JOBS_FLAG" ]]; then
        parallel_args=(--parallel "$JOBS_FLAG")
    fi
    echo "[run_examples] Building ${target}" | tee -a "$LOG_FILE"
    if ! cmake --build "$BUILD_DIR" --target "$target" "${parallel_args[@]}" >> "$LOG_FILE" 2>&1; then
        echo "[run_examples] Build failed for ${target}" | tee -a "$LOG_FILE"
        return 1
    fi
    return 0
}

run_example() {
    local name="$1"
    local binary="${BUILD_DIR}/examples/example_${name}"
    if [[ ! -x "$binary" ]]; then
        echo "[run_examples] Skipping run for ${name} (binary not found)" | tee -a "$LOG_FILE"
        return 0
    fi
    echo "[run_examples] Executing ${binary}" | tee -a "$LOG_FILE"
    if ! "$binary" >> "$LOG_FILE" 2>&1; then
        echo "[run_examples] Runtime failed for ${name}" | tee -a "$LOG_FILE"
        return 1
    fi
    return 0
}

collect_examples() {
    mapfile -d '' EXAMPLES < <(find "$ROOT_DIR/examples" -mindepth 1 -maxdepth 1 -type d -print0 | sort -z)
    if [[ -n "$FILTER_PATTERN" ]]; then
        local filtered=()
        for dir in "${EXAMPLES[@]}"; do
            local name="$(basename "$dir")"
            case "$name" in
                $FILTER_PATTERN)
                    filtered+=("$dir")
                    ;;
            esac
        done
        EXAMPLES=(${filtered[@]:-})
    fi
}

main() {
    collect_examples

    if [[ ${#EXAMPLES[@]} -eq 0 ]]; then
        echo "[run_examples] No examples matched filter '${FILTER_PATTERN}'" >&2
        exit 1
    fi

    if $LIST_ONLY; then
        printf '%s\n' "${EXAMPLES[@]##*/}"
        exit 0
    fi

    configure_if_needed

    local build_failures=0
    local run_failures=0

    for dir in "${EXAMPLES[@]}"; do
        local example_name="$(basename "$dir")"
        if ! build_example "$example_name"; then
            build_failures=$((build_failures + 1))
            continue
        fi

        if $EXECUTE; then
            if ! run_example "$example_name"; then
                run_failures=$((run_failures + 1))
            fi
        fi
    done

    if [[ $build_failures -ne 0 || $run_failures -ne 0 ]]; then
        echo "[run_examples] Completed with ${build_failures} build failure(s) and ${run_failures} runtime failure(s). See ${LOG_FILE}." >&2
        exit 1
    fi

    echo "[run_examples] All selected example targets processed successfully." | tee -a "$LOG_FILE"
}

main
