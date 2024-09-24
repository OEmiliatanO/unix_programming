#/bin/bash

# Test 1:
./cp cp.c cp_test1.c
diff cp.c cp_test1.c

# Test 2:
echo "123" > test1.txt
./cp test1.txt test2.txt
diff test1.txt test2.txt

# Test 3:
./cp hole_file/file_with_holes hole_file/file_with_holes.copy
diff hole_file/file_with_holes hole_file/file_with_holes.copy
