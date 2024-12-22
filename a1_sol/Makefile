# MARKING TIP:
# 1. Rename this makefile to makefile.instructor
# 2. Copy this rename makefile, markingTest.py, and testDriver.c into student folder
# 3. Run with:
#      make -f makefile.instructor

all: buildTests runTests

buildTests: list.c list.h testDriver.c
	gcc -g -std=c11 -o instructorTests list.c testDriver.c -Wall

runTests:
	python3 ./markingTest.py ./instructorTests

clean:
	rm -f instructorTests
