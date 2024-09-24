#!/bin/bash

# test 1
echo "Test 1: ls -l | ./tee test1.txt test2.txt test3.txt"
ls -l | ./tee test1.txt test2.txt test3.txt
diff test1.txt test2.txt
diff test1.txt test3.txt
diff test2.txt test3.txt
echo ""

# test 2
echo "Test 2: pwd | ./tee test1.txt test2.txt test3.txt"
pwd | ./tee test1.txt test2.txt test3.txt
diff test1.txt test2.txt
diff test1.txt test3.txt
diff test2.txt test3.txt
echo ""

# test 3
echo "Test 3: echo \"I love National Sun Yat-sen University, poor CSE\" | ./tee test1.txt test2.txt test3.txt"
echo "I love National Sun Yat-sen University, poor CSE" | ./tee test1.txt test2.txt test3.txt
diff test1.txt test2.txt
diff test1.txt test3.txt
diff test2.txt test3.txt
echo ""

# test 4
echo "Test 4: ls -l | ./tee test1.txt test2.txt test3.txt -a"
ls -l | ./tee test1.txt test2.txt test3.txt -a
diff test1.txt test2.txt
diff test1.txt test3.txt
diff test2.txt test3.txt
echo ""

# test 5
echo "Test 5: pwd | ./tee test1.txt test2.txt -a test3.txt"
pwd | ./tee test1.txt test2.txt -a test3.txt
diff test1.txt test2.txt
diff test1.txt test3.txt
diff test2.txt test3.txt
echo ""

# test 6
echo "Test 6: echo \"I love National Sun Yat-sen University, poor CSE\" | ./tee test1.txt -a test2.txt test3.txt"
echo "I love National Sun Yat-sen University, poor CSE" | ./tee test1.txt -a test2.txt test3.txt
diff test1.txt test2.txt
diff test1.txt test3.txt
diff test2.txt test3.txt
echo ""

# test 7
echo "Test 7: echo \"zzzzz\" | ./tee -a test1.txt test2.txt test3.txt"
echo "zzzzz" | ./tee test1.txt -a test2.txt test3.txt
diff test1.txt test2.txt
diff test1.txt test3.txt
diff test2.txt test3.txt
