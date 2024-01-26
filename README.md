#### libpam-basicauth

build&install:


	# Debian 12
	apt install libpam0g-dev libcurl4
	make build && make install


usage:


	# /etc/pam.d/<pam_service_name>
	auth required pam_basicauth.so url=<authentication_url> [user_agent=<custom_user_agent>]


license:


	MIT License
