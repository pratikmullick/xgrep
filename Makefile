prog = xgrep

$(prog): $(prog).o
	cc -o $@ $? ../XRE/linux/libxre.a

$(prog).o: $(prog).c
	cc -c -o $@ $?
