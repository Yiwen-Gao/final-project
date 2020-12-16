# final-project
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
