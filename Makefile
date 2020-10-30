prog = xgrep

$(prog): $(prog).o
	clang -o $@ $? ../XRE/linux/libxre.a

$(prog).o: $(prog).c
	clang -c -o $@ $?
