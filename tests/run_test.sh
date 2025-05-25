#!/bin/sh

GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
CYAN="\033[0;36m"
RESET="\033[0m"

test_dir="$1"
patchw="$2"
test_args="$3"

if [ ! -d "$test_dir" ]; then
  echo -e "${RED}Error: Test directory '$test_dir' not found${RESET}"
  exit 1
fi

echo -e "Running test: ${YELLOW}$test_dir${RESET}"

# Check if test is marked as pending
if [ -f "$test_dir/pending.txt" ]; then
  echo -e "${CYAN}PENDING${RESET}: Test marked as pending"
  if [ -f "$test_dir/pending.txt" ] && [ -s "$test_dir/pending.txt" ]; then
    echo -e "${BLUE}Reason: $(cat "$test_dir/pending.txt")${RESET}"
  fi
  exit 0
fi

expected_fail=0
if [ -f "$test_dir/expect-fail.txt" ]; then
  expected_fail=1
  echo -e "${BLUE}Note: This test is expected to fail${RESET}"
fi

if [ -f "$test_dir/source.txt" ]; then
  cp "$test_dir/source.txt" "$test_dir/inplace.txt"
  chmod ug+w "$test_dir/inplace.txt"
fi

cd "$test_dir"
$patchw $test_args ./patch.txt
exit_code=$?

function prettyPrintFiles {
  ../pretty-print.sh "../$test_dir"
}

if [ $exit_code -eq 1 ]; then
  prettyPrintFiles
  echo -e "${RED}FAILED${RESET}: Assertion"
  exit 1
elif [ $exit_code -eq 2 ]; then
  prettyPrintFiles
  echo -e "${RED}FAILED${RESET}: Unimplemented"
  exit 1
elif [ $expected_fail -eq 1 ]; then
  if [ $exit_code -eq 139 ] || [ $exit_code -eq 134 ] || [ $exit_code -eq 136 ] || [ $exit_code -eq 137 ] || [ $exit_code -eq 138 ]; then
    prettyPrintFiles
    echo -e "${RED}FAILED${RESET} $test_dir: Critical failure (exit code $exit_code) - segmentation fault or similar error"
    exit 1
  elif [ $exit_code -ne 0 ]; then
    echo -e "${GREEN}PASSED${RESET}: Test failed as expected with exit code $exit_code"
    exit 0
  else
    if diff -q inplace.txt expect.txt > /dev/null; then
      prettyPrintFiles
      echo -e "${RED}FAILED${RESET} $test_dir: Test was expected to fail but succeeded"
      exit 1
    else
      echo -e "${GREEN}PASSED${RESET}: Test output differs from expected as intended"
      exit 0
    fi
  fi
else
  if [ $exit_code -ne 0 ]; then
    prettyPrintFiles
    echo -e "${RED}FAILED${RESET} $test_dir: patchw returned error code $exit_code"
    exit 1
  elif diff -q inplace.txt expect.txt > /dev/null; then
    echo -e "${GREEN}PASSED${RESET}"
    exit 0
  else
    prettyPrintFiles
    echo -e "${RED}FAILED${RESET} $test_dir: Output doesn't match expected expect"
    echo "Expected:"
    cat expect.txt
    echo "Got:"
    cat inplace.txt
    exit 1
  fi
fi
