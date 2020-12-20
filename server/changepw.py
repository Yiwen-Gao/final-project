#!/usr/bin/python3

import dbmanager  
import sys 

def main():
    if len(sys.argv) < 3:
        print('usage: python3 changepw.py <username> <password>')
        sys.exit(1)

    username, password = sys.argv[1:]
    dbmanager.update_user(username, password)
    print('changepw: changed password')

main()