#!/bin/bash

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Comprehensive Test Script for life
echo -e "${BLUE}🔍 Running COMPREHENSIVE TESTING for life${NC}"
echo "=========================================="
echo ""

# Compile the reference solution
echo -e "${BLUE}📦 Compiling reference solution...${NC}"
gcc -Wall -Wextra -Werror -std=c99 -o ref_life main.c life.c

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Reference compilation failed!${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Reference compilation successful!${NC}"
echo ""

# Check if user solution files exist
USER_C_FILES=$(find ../../../../rendu/life/ -name "*.c" 2>/dev/null)
USER_H_FILES=$(find ../../../../rendu/life/ -name "*.h" 2>/dev/null)

if [ -z "$USER_C_FILES" ] || [ -z "$USER_H_FILES" ]; then
    echo -e "${RED}❌ User solution not found: No .c or .h files in ../../../../rendu/life/${NC}"
    exit 1
fi

# Copy and compile user solution
echo -e "${BLUE}📦 Compiling user solution...${NC}"
cp main.c user_main.c
cp -r ../../../../rendu/life/* .
gcc -Wall -Wextra -Werror -std=c99 -o user_life *.c
if [ $? -ne 0 ]; then
    echo -e "${RED}❌ User compilation failed!${NC}"
    exit 1
fi

echo -e "${GREEN}✅ User compilation successful!${NC}"
echo ""

# Test 1: Basic test from subject
echo -e "${BLUE}🚀 Running Test 1: Basic square pattern (0 iterations)...${NC}"
echo 'sdxddssaaww' | ./ref_life 5 5 0 > ref_output1.txt 2>&1
echo "[DEBUG] Reference output for test1:"; cat -e ref_output1.txt
echo 'sdxddssaaww' | ./user_life 5 5 0 > user_output1.txt 2>&1
echo "[DEBUG] User output for test1:"; cat -e user_output1.txt

test1_match=true
if diff -q ref_output1.txt user_output1.txt > /dev/null; then
    echo -e "${GREEN}✅ Test 1 output matches reference!${NC}"
else
    echo -e "${RED}❌ Test 1 output does NOT match reference!${NC}"
    echo -e "${YELLOW}--- Reference Output ---${NC}"
    cat -e ref_output1.txt
    echo -e "${YELLOW}--- Your Output ---${NC}"
    cat -e user_output1.txt
    echo -e "${YELLOW}--- Diff ---${NC}"
    diff ref_output1.txt user_output1.txt
    test1_match=false
fi

# Test 2: Complex pattern test
echo -e "${BLUE}🚀 Running Test 2: Complex pattern (0 iterations)...${NC}"
echo 'sdxssdswdxddddsxaadwxwdxwaa' | ./ref_life 10 6 0 > ref_output2.txt 2>&1
echo "[DEBUG] Reference output for test2:"; cat -e ref_output2.txt
echo 'sdxssdswdxddddsxaadwxwdxwaa' | ./user_life 10 6 0 > user_output2.txt 2>&1
echo "[DEBUG] User output for test2:"; cat -e user_output2.txt

test2_match=true
if diff -q ref_output2.txt user_output2.txt > /dev/null; then
    echo -e "${GREEN}✅ Test 2 output matches reference!${NC}"
else
    echo -e "${RED}❌ Test 2 output does NOT match reference!${NC}"
    test2_match=false
fi

# Test 3: Vertical line (0 iterations)
echo -e "${BLUE}🚀 Running Test 3: Vertical line (0 iterations)...${NC}"
echo 'dxss' | ./ref_life 3 3 0 > ref_output3.txt 2>&1
echo "[DEBUG] Reference output for test3:"; cat -e ref_output3.txt
echo 'dxss' | ./user_life 3 3 0 > user_output3.txt 2>&1
echo "[DEBUG] User output for test3:"; cat -e user_output3.txt

test3_match=true
if diff -q ref_output3.txt user_output3.txt > /dev/null; then
    echo -e "${GREEN}✅ Test 3 output matches reference!${NC}"
else
    echo -e "${RED}❌ Test 3 output does NOT match reference!${NC}"
    test3_match=false
fi

# Test 4: Game of Life evolution (1 iteration)
echo -e "${BLUE}🚀 Running Test 4: Game evolution (1 iteration)...${NC}"
echo 'dxss' | ./ref_life 3 3 1 > ref_output4.txt 2>&1
echo "[DEBUG] Reference output for test4:"; cat -e ref_output4.txt
echo 'dxss' | ./user_life 3 3 1 > user_output4.txt 2>&1
echo "[DEBUG] User output for test4:"; cat -e user_output4.txt

test4_match=true
if diff -q ref_output4.txt user_output4.txt > /dev/null; then
    echo -e "${GREEN}✅ Test 4 output matches reference!${NC}"
else
    echo -e "${RED}❌ Test 4 output does NOT match reference!${NC}"
    test4_match=false
fi

# Test 5: Game of Life evolution (2 iterations)
echo -e "${BLUE}🚀 Running Test 5: Game evolution (2 iterations)...${NC}"
echo 'dxss' | ./ref_life 3 3 2 > ref_output5.txt 2>&1
echo "[DEBUG] Reference output for test5:"; cat -e ref_output5.txt
echo 'dxss' | ./user_life 3 3 2 > user_output5.txt 2>&1
echo "[DEBUG] User output for test5:"; cat -e user_output5.txt

test5_match=true
if diff -q ref_output5.txt user_output5.txt > /dev/null; then
    echo -e "${GREEN}✅ Test 5 output matches reference!${NC}"
else
    echo -e "${RED}❌ Test 5 output does NOT match reference!${NC}"
    test5_match=false
fi

# Test 6: Empty input
echo -e "${BLUE}🚀 Running Test 6: Empty input...${NC}"
echo '' | ./ref_life 3 3 0 > ref_output6.txt 2>&1
echo "[DEBUG] Reference output for test6:"; cat -e ref_output6.txt
echo '' | ./user_life 3 3 0 > user_output6.txt 2>&1
echo "[DEBUG] User output for test6:"; cat -e user_output6.txt

test6_match=true
if diff -q ref_output6.txt user_output6.txt > /dev/null; then
    echo -e "${GREEN}✅ Test 6 output matches reference!${NC}"
else
    echo -e "${RED}❌ Test 6 output does NOT match reference!${NC}"
    test6_match=false
fi

# Run with valgrind for memory leak checking
echo -e "${BLUE}🚀 Executing valgrind analysis...${NC}"
echo "Command: valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./user_life 5 5 0"
echo ""

# Capture valgrind output to analyze
valgrind_output=$(echo 'sdxddssaaww' | valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    -s \
    --error-exitcode=1 \
    ./user_life 5 5 0 2>&1)

exit_code=$?

# Display the full output
echo "$valgrind_output"

echo ""
echo -e "${BLUE}🏁 Valgrind analysis completed with exit code: $exit_code${NC}"
echo ""

# Parse and analyze the output
echo "======================================="
echo -e "${YELLOW}📊 DETAILED ANALYSIS RESULTS:${NC}"
echo "======================================="

# Check for memory leaks
has_leaks=false
if echo "$valgrind_output" | grep -q "definitely lost:" && echo "$valgrind_output" | grep "definitely lost:" | grep -v "0 bytes"; then
    has_leaks=true
fi
if echo "$valgrind_output" | grep -q "indirectly lost:" && echo "$valgrind_output" | grep "indirectly lost:" | grep -v "0 bytes"; then
    has_leaks=true
fi
if echo "$valgrind_output" | grep -q "possibly lost:" && echo "$valgrind_output" | grep "possibly lost:" | grep -v "0 bytes"; then
    has_leaks=true
fi

# Check for errors
has_errors=false
if echo "$valgrind_output" | grep -q "ERROR SUMMARY" && echo "$valgrind_output" | grep "ERROR SUMMARY" | grep -v "0 errors"; then
    has_errors=true
fi

# Display results with color coding
echo -n "Test 1 (Basic square): "
if [ "$test1_match" = true ]; then
    echo -e "${GREEN}PASSED${NC}"
else
    echo -e "${RED}FAILED${NC}"
fi

echo -n "Test 2 (Complex pattern): "
if [ "$test2_match" = true ]; then
    echo -e "${GREEN}PASSED${NC}"
else
    echo -e "${RED}FAILED${NC}"
fi

echo -n "Test 3 (Vertical line): "
if [ "$test3_match" = true ]; then
    echo -e "${GREEN}PASSED${NC}"
else
    echo -e "${RED}FAILED${NC}"
fi

echo -n "Test 4 (1 iteration): "
if [ "$test4_match" = true ]; then
    echo -e "${GREEN}PASSED${NC}"
else
    echo -e "${RED}FAILED${NC}"
fi

echo -n "Test 5 (2 iterations): "
if [ "$test5_match" = true ]; then
    echo -e "${GREEN}PASSED${NC}"
else
    echo -e "${RED}FAILED${NC}"
fi

echo -n "Test 6 (Empty input): "
if [ "$test6_match" = true ]; then
    echo -e "${GREEN}PASSED${NC}"
else
    echo -e "${RED}FAILED${NC}"
fi

echo -n "Memory Leaks: "
if [ "$has_leaks" = true ]; then
    echo -e "${RED}DETECTED - You have memory leaks!${NC}"
else
    echo -e "${GREEN}PASSED - No memory leaks detected${NC}"
fi

echo -n "Valgrind Errors: "
if [ "$has_errors" = true ]; then
    echo -e "${RED}DETECTED - Valgrind found errors!${NC}"
else
    echo -e "${GREEN}PASSED - No valgrind errors${NC}"
fi

echo ""
echo "======================================="
echo -n "OVERALL RESULT: "
all_tests_passed=true
if [ "$test1_match" = false ] || [ "$test2_match" = false ] || [ "$test3_match" = false ] || [ "$test4_match" = false ] || [ "$test5_match" = false ] || [ "$test6_match" = false ] || [ "$has_leaks" = true ] || [ "$has_errors" = true ]; then
    all_tests_passed=false
fi

if [ "$all_tests_passed" = true ]; then
    echo -e "${GREEN}✅ ALL TESTS PASSED!${NC}"
    echo -e "${GREEN}Your life implementation is clean!${NC}"
else
    echo -e "${RED}❌ ISSUES DETECTED!${NC}"
    echo -e "${YELLOW}Summary of errors:${NC}"
    if [ "$test1_match" = false ]; then
        echo -e "${RED}  → Test 1 (Basic square) failed.${NC}"
    fi
    if [ "$test2_match" = false ]; then
        echo -e "${RED}  → Test 2 (Complex pattern) failed.${NC}"
    fi
    if [ "$test3_match" = false ]; then
        echo -e "${RED}  → Test 3 (Vertical line) failed.${NC}"
    fi
    if [ "$test4_match" = false ]; then
        echo -e "${RED}  → Test 4 (1 iteration) failed.${NC}"
    fi
    if [ "$test5_match" = false ]; then
        echo -e "${RED}  → Test 5 (2 iterations) failed.${NC}"
    fi
    if [ "$test6_match" = false ]; then
        echo -e "${RED}  → Test 6 (Empty input) failed.${NC}"
    fi
    if [ "$has_leaks" = true ]; then
        echo -e "${RED}  → Memory leaks detected.${NC}"
    fi
    if [ "$has_errors" = true ]; then
        echo -e "${RED}  → Valgrind errors detected.${NC}"
    fi
fi
echo "======================================="

# Wait for user to press enter before continuing
read -rp "Press enter to continue..." dummy

# Cleanup temporary files
rm -f ref_life user_life user_main.c *.h *.c ref_output*.txt user_output*.txt
# Don't remove original files from user's rendu folder