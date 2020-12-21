#!/usr/bin/python3

import csv
import dbmanager

data = []
with open('bellovin/userinfo.txt') as f:
    data = [row.split() for row in f]

users = [None] * len(data)
for i, row in enumerate(data):
    username, _, password = row
    salt, hashed_password = dbmanager.hash(password)
    users[i] = dbmanager.User(username, salt, hashed_password)

dbmanager.update_all_users(users)