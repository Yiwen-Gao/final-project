#!/usr/bin/python3

import dbmanager 
import sys  

def main():
    if len(sys.argv) < 3:
        print('usage: python3 verifypw.py <username> <password>')
        sys.exit(1)

    username, password = sys.argv[1:]
    user = dbmanager.select_user(username)
    _, hashed_password = dbmanager.hash(password, user.salt)
    if user.hashed_password == hashed_password:
        print('verifypw: correct password')
        sys.exit(0)
    else:
        print('verifypw: incorrect password')
        sys.exit(1)

main()