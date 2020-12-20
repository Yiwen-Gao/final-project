#!/usr/bin/python3
# code to generate data in user-info.txt 

import crypt
import secrets
import sys

with open('/usr/share/dict/words') as f:
    words = [word.strip() for word in f]

for i in sys.argv[1:]:
	pw = '_'.join(secrets.choice(words) for i in range(2)) # password is random combination of two other usernames
	hpw = crypt.crypt(pw, salt=None) # hash and salt the password (if salt is None, crypt will use a default salt)
	print(i, hpw, pw)