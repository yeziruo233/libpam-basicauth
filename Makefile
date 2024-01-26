build:
	gcc -fPIC -shared module.c -o pam_basicauth.so -lcurl

install:
	mkdir /lib/security/
	cp pam_basicauth.so /lib/security/
