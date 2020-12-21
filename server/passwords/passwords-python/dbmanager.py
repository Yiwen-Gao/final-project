#!/usr/bin/python3
# modified from Bellovin's code

import crypt
import csv
import secrets
import string
import sys

DB_NAME = 'passwords.csv'
COL_HEADERS = ['username', 'salt', 'hashed_password']

class User:
    def __init__(self, username, salt, hashed_password):
        self.username = username
        self.salt = salt
        self.hashed_password = hashed_password

def hash(password, salt=None):
    if salt is None:
        salt = crypt.mksalt()

    hashed_password = crypt.crypt(password, salt=salt)
    return salt, hashed_password

def select_user(username):
    users = select_all_users()
    for entry in users:
        if entry.username == username:
            return entry

def update_user(username, password):
    salt, hashed_password = hash(password)
    users = select_all_users()
    for entry in users:
        if entry.username == username:
            entry.salt = salt
            entry.hashed_password = hashed_password

    update_all_users(users)

def select_all_users():
    users = []
    with open(DB_NAME) as f:
        reader = csv.reader(f, delimiter=',')
        data = list(reader)
        for i, row in enumerate(data):
            if i > 0:
                users.append(User(*row))

    return users

def update_all_users(users):
    rows = [[u.username, u.salt, u.hashed_password] for u in users]
    with open(DB_NAME, 'w') as f:
        writer = csv.writer(f)
        writer.writerow(COL_HEADERS)
        writer.writerows(rows)
