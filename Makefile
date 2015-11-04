des: main.c data.c
	gcc -o des -g data.c main.c

test: des
	./des enc 0123456789ABCDEF 23456789ABCDEF01 456789ABCDEF0123 < tests/1.in | cmp - tests/1.out
	./des dec 0123456789ABCDEF 23456789ABCDEF01 456789ABCDEF0123 < tests/1.out | cmp - tests/1.in
	@echo "### OK ###"
