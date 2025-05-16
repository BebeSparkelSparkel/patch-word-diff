#!/bin/sh

# Colors for formatting
YELLOW="\033[0;33m"
GREEN="\033[0;32m"
BLUE="\033[0;34m"
CYAN="\033[0;36m"
RED="\033[0;31m"
BOLD="\033[1m"
RESET="\033[0m"

TEST_DIR="$1"

if [ -z "$TEST_DIR" ]; then
  echo -e "${RED}Error: Please specify a test directory${RESET}"
  echo "Usage: $0 <test_directory>"
  echo "Example: $0 100_empty-source-addition-patch"
  exit 1
fi

if [ ! -d "$TEST_DIR" ]; then
  echo -e "${RED}Error: Test directory '$TEST_DIR' not found${RESET}"
  exit 1
fi

echo -e "${BOLD}${YELLOW}Test: ${TEST_DIR}${RESET}"
echo

if [ -f "${TEST_DIR}/source.txt" ]; then
  echo -e "${BOLD}${BLUE}Source:${RESET}"
  echo -e "${CYAN}------------------${RESET}"
  sed 's/^/  /' "${TEST_DIR}/source.txt"
  echo -e "${CYAN}------------------${RESET}"
  echo
else
  echo -e "${BLUE}Source: ${RED}Not available${RESET}"
  echo
fi

if [ -f "${TEST_DIR}/patch.txt" ]; then
  echo -e "${BOLD}${BLUE}Patch:${RESET}"
  echo -e "${CYAN}------------------${RESET}"
  sed 's/^/  /' "${TEST_DIR}/patch.txt"
  echo -e "${CYAN}------------------${RESET}"
  echo
else
  echo -e "${BLUE}Patch: ${RED}Not available${RESET}"
  echo
fi

if [ -f "${TEST_DIR}/result.txt" ]; then
  echo -e "${BOLD}${BLUE}Expected Result:${RESET}"
  echo -e "${CYAN}------------------${RESET}"
  sed 's/^/  /' "${TEST_DIR}/result.txt"
  echo -e "${CYAN}------------------${RESET}"
  echo
else
  echo -e "${BLUE}Expected Result: ${RED}Not available${RESET}"
  echo
fi

if [ -f "${TEST_DIR}/expect-fail.txt" ]; then
  echo -e "${BOLD}${RED}Note: This test is expected to fail${RESET}"
fi
