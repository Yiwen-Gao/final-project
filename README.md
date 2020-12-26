# Secure Mail System
This project allows a predefined set of users to securely mail messages to each other.

## Installation
`$ git clone https://github.com/Yiwen-Gao/final-project.git`

`$ sudo apt-get install libssl-dev`

`$ cd final-project`

`$ ./setupmailsystem.sh <mail_system_name>`

## Usage
Start the server.

`$ cd <mail_system_name>/server`

`$ sudo ./server`

Open a new window to run the client programs.

`$ cd <mail_system_name>/client`

### 1) getcert
Retrieve a client certificate.

`$ ./getcert <username>`

At the first prompt, enter the user password. At the second prompt, enter the word `dummy`, which is the password for the dummy certificate.

### 2) changepw
Change the user's password and retrieve a new client certificate.

`$ ./changepw <username>`

At the first prompt, enter the old password. At the second prompt, enter your new password.

### 3) sendmsg
Send mail to a list of users.

`$ ./sendmsg <username>`

At the first prompt, follow the instructions to format the mail message. Enter the recipient usernames delimited by single new lines, a blank line, the message, and a period followed by a new line. For example,

```
addleness
analects
annalistic

hello friends!
.
```

At the following prompts, enter the user password.

### 4) recvmsg
Receive the most recent piece of mail.

`$ ./recvmsg <username>`

At both prompts, enter the user password.

## Testing
Tests are available in `testscripts/`.

## File Structure
All executables and data are inside `<mail_system_name>/`.

- `client/`: client-side executables and data
  - `bin/`: executables
    - `changepw`: change user password and retrieve new client certificate
    - `createcsr`: create certificate signing request for `getcert` and `changepw`
    - `getcert`: retrieve client certificate
    - `recvmsg`: receive most recent piece of mail
    - `sendmsg`: send mail to list of users
  - `certs/`: client certificates
  - `csr/`: client certificate signing requests and client keys
  - `dummy/`: certificate authentication for users without their own certificate
    - `dummy.cert.pem`: dummy certificate for `getcert` 
    - `dummy.key.pem`: dummy key for `getcert`
  - `trusted-certs/`: predefined trusted certificates
    - `ca-chain.cert.pem`: concatenated intermediate and root certificates 
    - `ca.cert.pem`: root certificate
    - `intermediate.cert.pem`: intermediate certificate signed by CA 
- `server/`: server-side executables and data
  - `ca/`: certificate authentication
    - `bin/`: executables
      - `get-cert`: create new client certificate
      - `issueservercert.sh`: create server certificate
      - `setupca.sh`: set up `server/ca/ca/` directory
      - `signcsr.sh`: sign certificate request
    - `ca/`: data storage (following list only contains select items)
      - `certs/ca.cert.pem`: root certificate
      - `intermediate/certs/`: CA chain, intermediate, dummy, and client certificates
    - `casetupinput.txt`: CA setup input file
    - `dummyinput.txt`: dummy certificate input file
    - `intermediateopenssl.cnf`: intermediate certificate configuration
    - `openssl.cnf`: CA configuration
  - `mail/`: read and write messages
    - `bin/`: executables
      - `get-msg`: read message from user mailbox
    - `mail/`: user mailboxes
  - `password/`: password authentication
    - `bin/`: executables
      - `add-user`: add new user to mail system
      - `change-pw`: change user password
      - `crypt-pw`: initialize database with predefined usernames and passwords 
      - `verify-pw`: verify user password
    - `passwords.txt`: database of usernames, salts, and hashed/salted passwords
  - `server`: accept and respond to client requests

## Permissions

## Containerization

