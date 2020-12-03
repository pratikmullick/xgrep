prog = xgrep

$(prog): $(prog).o
	clang -o $@ $? /usr/local/lib/libxre.a

$(prog).o: $(prog).c
	clang -c -o $@ $?
