des: main.c des.c data.c des.h
	gcc -o des -g data.c des.c main.c

des_test_suite: des.c data.c des.h check_des.c
	gcc -o des_test_suite -g data.c des.c check_des.c -lcheck

test: unit_tests functional_tests

functional_tests: des
	./des enc 0123456789ABCDEF 23456789ABCDEF01 456789ABCDEF0123 < tests/1.in | cmp - tests/1.out
	./des dec 0123456789ABCDEF 23456789ABCDEF01 456789ABCDEF0123 < tests/1.out | cmp - tests/1.in
	@echo "### OK ###"

unit_tests: des_test_suite
	./des_test_suite
