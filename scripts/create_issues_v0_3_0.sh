#!/usr/bin/env bash
set -euo pipefail

# Create GitHub milestone and issues for v0.3.0 from a fixed list.
# Requirements:
#  - gh CLI installed: https://cli.github.com
#  - gh auth login (with repo scope)
#  - Run from repository root (PiPinPP)

REPO="Barbatos6669/PiPinPP"
MILESTONE_TITLE="v0.3.0"
LABELS="enhancement,roadmap,v0.3.0"

check_gh() {
  if ! command -v gh >/dev/null 2>&1; then
    echo "Error: gh CLI not found. Install from https://cli.github.com and run 'gh auth login'" >&2
    exit 1
  fi
  if ! gh auth status >/dev/null 2>&1; then
    echo "Error: gh not authenticated. Run 'gh auth login'" >&2
    exit 1
  fi
}

create_milestone() {
  echo "Creating (or finding) milestone: $MILESTONE_TITLE"
  local mid
  mid=$(gh api \
    -H "Accept: application/vnd.github+json" \
    "/repos/${REPO}/milestones" --paginate | \
    jq -r ".[] | select(.title==\"${MILESTONE_TITLE}\") | .number" | head -n1 || true)

  if [[ -z "${mid}" ]]; then
    gh api -X POST \
      -H "Accept: application/vnd.github+json" \
      "/repos/${REPO}/milestones" \
      -f title="${MILESTONE_TITLE}" \
      -f state=open >/dev/null
    echo "Milestone created."
  else
    echo "Milestone exists (#${mid})."
  fi
}

create_issue() {
  local title="$1" body="$2"
  gh issue create \
    --repo "${REPO}" \
    --title "$title" \
    --body "$body" \
    --label "$LABELS" \
    --milestone "$MILESTONE_TITLE"
}

main() {
  check_gh
  create_milestone

  echo "Creating issues..."

  create_issue "Thread-safety for ArduinoCompat" "Protect globalPins with a mutex.\n- Add std::mutex and guard map access\n- Document thread-safety notes\n- Minimal concurrent access test"

  create_issue "Tidy HIGH/LOW constants" "Replace global macros with Arduino::constexpr.\n- Keep deprecated aliases for one minor version\n- Update examples"

  create_issue "Logging + build options" "Add minimal logging macros and CMake options.\n- PIPINPP_ENABLE_LOGGING, PIPINPP_LOG_LEVEL\n- Add -Wall -Wextra -Wpedantic (optionally as errors)\n- Remove std::cout from library code"

  create_issue "Repo discoverability" "Improve GitHub presence.\n- Add topics: raspberry-pi, gpio, cpp, arduino, libgpiod, embedded\n- Add README badges (build, release, license)\n- Optional GitHub Pages for docs"

  create_issue "Docs and wiki updates" "Add pin numbering guide and permissions guide.\n- BCM vs physical mapping\n- gpio group / udev rules\n- TOC anchors\n- Keep wiki in sync"

  create_issue "Timing functions (millis/micros)" "Implement millis(), micros(), delayMicroseconds().\n- Use steady_clock\n- Unit tests for monotonicity and bounds"

  create_issue "Custom exceptions" "Introduce PinError, InvalidPinError, GpioAccessError.\n- Replace generic throws\n- Improve messages\n- Document error model"

  create_issue "GitHub Actions CI" "Build + test matrix, pkg-config smoke test.\n- Optional CodeQL\n- Add badges to README"

  create_issue "Unit tests (GoogleTest)" "Add GTest and mockable libgpiod interface.\n- Tests for Pin, ArduinoCompat, timing"

  create_issue "New examples" "Add interrupt_button, pwm_led_fade, timing_benchmark.\n- Include wiring notes"

  create_issue "CMake packaging polish" "Add shared library option and install CMake package config.\n- Export targets\n- Verify CMake and pkg-config consumption paths"

  create_issue "Implement interrupts API" "Add attachInterrupt/detachInterrupt with libgpiod events.\n- RISING/FALLING/CHANGE\n- Internal event thread with safe shutdown\n- Example + docs"

  create_issue "Add PWM (analogWrite)" "Provide software PWM backend (configurable freq/duty).\n- Document jitter\n- LED fade example\n- Roadmap to hardware PWM backend"

  create_issue "Namespace core API" "Move to pipinpp:: namespace with transitional using-decls.\n- Update headers and docs"

  echo "All issues created (or attempted)."
}

main "$@"
