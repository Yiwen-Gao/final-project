# final-project

## Dependencies
Install the following libraries before starting the CA, client, or server.
`$ sudo apt-get install libssl-dev`

## CA
What I've added:
-Makefile
-TLS client starter code
-demos (these are how to use the SSL routines, so we all have access to them)
-CATools containing all the stuff necessary for the CA.

How to use the CA:
-This site: http://h30266.www3.hpe.com/odl/axpos/opsys/vmsos84/BA554_90007/ch04s03.html gives info on how to
set up the TLS client and server using the SSL certificates. Basically you'll load them in 
specifying the paths to the different certificates.

-You'd first want to run ./CASetup to actually set up the CA with the CA and intermediate certs. Then,
you'd want to run ./issueServerCert and ./issueClientCert to issue the client and server certificates
to use for the client and server.

-When running ./CASetup, you need to enter something for the common names. I enter RootCA for the root CA and
IntermediateCA for the intermediate one. 

-when setting up the client and server certificates, you'd want to enter something as a command line argument
to identify the certificates, enter that same thing as the common name.

-feel free to replace anything I've done from your HW2 implementation, I'm sure there's things that could
be improved upon

-the password for the certificates is currently password123. I don't know how this should necessarily be done. 
Should we generate a new password for the certificates every time and store that somewhere?

-To generate the certificates to be used for the users, run ./issueEncryptionCert. I wasn't sure 
exactly what type of certificate it should be. Piazza says "You want a TLS+encryption+signing certificate." But 
I wasn't sure exactly what this translated to. In the [encyption_cert] section of the intermediate
config file (intermediateopenss.cnf), I added digitalSignature, and it already had dataEncipherment. I'm
not sure how to specify that it's a TLS certificate. 

## Server
-bellovin/: Bellovin's provided usernames and passwords 

-users.csv: formatted storage of usernames, salts, and passwords

-usergen.py: update users.csv with fresh copy of original usernames and passwords 

usage: `$ python3 usergen.py`

-changepw.py: change a user's password 

usage: `$ python3 changepw.py <username> <password>`

-verifypw.py: verify a user's password 

usage: `$ python3 verifypw.py <username> <password>`
