# Secure Mail System
This project allows a predefined set of users to send and receive mail from each other.

## Installation
`$ git clone https://github.com/Yiwen-Gao/final-project.git`
`$ sudo apt-get install libssl-dev`
`$ cd final-project`
`$ ./setupmailsystem.sh <base_directory_name>`

## Usage
`$ cd <base_directory_name>`

Start the server
`$ sudo ./server`

Open a new window to run the client programs
`$ cd client`

#### getcert
Retrieve a client certificate
`$ ./getcert <username>`
At the first prompt, enter the user password
At the second prompt, enter `dummy`--the password for the dummy certificate 

#### changepw
Change user's password and retrieve a new client certificate
`$ ./changepw <username>`

#### sendmsg
Send mail to a list of users
`$ ./sendmsg <username>`

#### recvmsg
Receive the most recent piece of mail
`$ ./recvmsg <username>`

## Testing
Tests are available in `testscripts/`

## File Structure
Inside `<base_directory_name>/`
-- client/: client-side executables and data
    -- bin/: executables
        -- changepw: change user's password and retrieve a new client certificate
        -- createcsr: generate the certificate signing request for getcert and changepw
        -- getcert: retrieve a client certificate
        -- intermediateopenssl.cnf:
        -- recvmsg: receive the most recent piece of mail
        -- selfsigncert.sh:
        -- sendmsg: send mail to a list of users
    -- `MISSING` trusted-certs/:
        -- ca-chain.cert.pem: intermediate and root certificates concatenated together
        -- ca.cert.pem: CA root certificate
        -- intermediate.cert.pem: intermediate certificate signed by CA root certificate
    -- certs/: client certificates
    -- csr/: client certificate signing requests
        -- private/: client keys
    -- dummy/: 
        -- dummy.cert.pem: dummy certificate available to all users
        -- dummy.key.pem: dummy key available to all users
-- server/: server-side executables and data
    -- ca/: certificate authentication
        -- bin/: executables
            -- get-cert: 
            -- issueservercert.sh:
            -- setupca.sh:
            -- signcsr.sh:
        -- ca/: (unused items are excluded from the list below)
            -- certs/:
                -- ca.cert.pem:
            -- crl/:
        -- casetupinput.txt:
        -- dummyinput:txt:
        -- intermediateopenssl.cnf:
        -- openssl.cnf:
    -- mail/: read and write messages
        -- bin/: executables
            -- get-msg:
            -- `MISSING` mail-in:
            -- `MISSING` mail-out:
        -- mail/: user mailboxes
    -- password/: password authentication
        -- bin/: excecutables
            -- add-user: add new user to the mail system
            -- change-pw: change user's password
            -- crypt-pw: initialize database with predefined usernames and passwords 
            -- verify-pw: verify user's password
        -- passwords.txt: database of usernames, salts, and hashed and salted passwords
    -- server: accept and respond to client requests

## Permissions

## Containerization

