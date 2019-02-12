all: copy write_test

copy: copy.c
	gcc -o copy copy.c

write_test: write_test.c
	gcc -o write_test write_test.c

clean:
	rm -rf copy write_test out
