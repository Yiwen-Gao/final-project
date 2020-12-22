#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

/*
 * Compile with -lssl -lcrypto
 *
 */

const int DEFAULT_PORT = 443;

using namespace std;

int main(int argc, char **argv) {

	//added specific to getcert
	if (argc != 2)
	{
		cerr << "Must supply a username" << endl;
		exit(1);
	}

	string username = argv[1];
	string password = getpass("Enter password");
	
	SSL_CTX *ctx;
	SSL *ssl;
	const SSL_METHOD *meth;
	BIO *sbio;
	int err; char *s;

	int ilen;
	char ibuf[512];

	//generate the CSR:
	FILE *tempfp = fopen("temp", "wb");
	if (tempfp == NULL)
	{
		cerr << "unable to open temp file" << endl;
	}

	//create a temp file for input to the csr
	string newline = "\n";
	for (int i = 0; i < 5; i++)
	{
		fwrite(newline.c_str(), sizeof(char), 1, tempfp);
	}
	fwrite(username.c_str(), sizeof(char), username.length(), tempfp);

	fwrite(newline.c_str(), sizeof(char), 1, tempfp);
	fwrite(newline.c_str(), sizeof(char), 1, tempfp);
	fwrite(newline.c_str(), sizeof(char), 1, tempfp);

	fclose(tempfp);

	FILE *inputfp = fopen("./input.sh", "wb");
	if (inputfp == NULL)
	{
		cerr << "unable to open temp file" << endl;
	}

	string bash = "#!/bin/bash\n\n";
	fwrite(bash.c_str(), sizeof(char), bash.length(), inputfp);
	
	string command = "./createcsr " + username + " " + password + " " + "< temp\n";
	fwrite(command.c_str(), sizeof(char), command.length(), inputfp);

	chmod("input.sh", S_IXGRP | S_IXUSR | S_IXOTH | S_IRGRP | S_IRUSR | S_IWUSR); 

	fclose(inputfp);
	//now fork and exec to execute the csr generation
	pid_t pid = fork();
	int status;
	if (pid == -1)
	{
		cerr << "fork error" << endl;
		return 1;
	} else if (pid == 0)
	{
		execl("./input.sh", "input.sh", (char *)0);
		cerr << "execl failed" << endl;
		return 1;
	} else {
		if (waitpid(pid, &status, 0) > 0)
		{
			if (WIFEXITED(status) && WEXITSTATUS(status)){
				cerr << WEXITSTATUS(status) << endl;
				if (WEXITSTATUS(status) == 127) {
					cerr << "execv failed" << endl;
				}
			}
		} else {
			cerr << "waitpid failed" << endl;
		}
	}

	//need to delete the temp files when we're done
	if (remove("./input.sh") != 0)
	{
		cerr << "failed to remove temp file" << endl;
		return 1;
	}

	if (remove("temp") != 0)
	{
		cerr << "failed to remove temp file" << endl;
		return 1;
	}


	//Added to send the CSR to the server
	string CSRFILE = "./csr/" + username + ".pem";

	string toSend = "POST getcert HTTP/1.0\n"; 
	FILE *fp = fopen(CSRFILE.c_str(), "rb");
	if (fp == NULL)
	{
		cerr << "Failed to open csr" << endl;
		return 1;
	}
	
	int contentlength = username.length() + password.length() + 2;
	int n = 0;
	string csrContents = "";
	char buffer[40];
	memset(buffer, 0, 40);
	while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0)
	{
		contentlength += n;
		csrContents += buffer;
		memset(buffer, 0, 40);
	}
	fclose(fp);

	toSend += "Content-Length: ";
	toSend += to_string(contentlength);
	toSend += "\n";
	toSend += username + "\n";
	toSend += password + "\n";
	toSend += csrContents + "\n\n";

	//source code:
	const char *obuf = toSend.c_str();

	struct sockaddr_in sin;
	int sock;
	struct hostent *he;

	SSL_library_init(); /* load encryption & hash algorithms for SSL */         	
	SSL_load_error_strings(); /* load the error strings for good error reporting */

	meth = TLS_client_method();
	ctx = SSL_CTX_new(meth);

	SSL_CTX_set_default_verify_dir(ctx);
	//SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

	ssl = SSL_new(ctx);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("socket");
		return 1;
	}

	bzero(&sin, sizeof sin);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(DEFAULT_PORT);

	he = gethostbyname("localhost");//gethostbyname("www.cs.columbia.edu");
	memcpy(&sin.sin_addr, (struct in_addr *)he->h_addr, he->h_length);
	if (connect(sock, (struct sockaddr *)&sin, sizeof sin) < 0) {
		perror("connect");
		return 2;
	}

	sbio = BIO_new(BIO_s_socket());
	BIO_set_fd(sbio, sock, BIO_NOCLOSE);
	SSL_set_bio(ssl, sbio, sbio);

	err = SSL_connect(ssl);
	if (SSL_connect(ssl) != 1) {
		switch (SSL_get_error(ssl, err)) {
			case SSL_ERROR_NONE: s="SSL_ERROR_NONE"; break;
			case SSL_ERROR_ZERO_RETURN: s="SSL_ERROR_ZERO_RETURN"; break;
			case SSL_ERROR_WANT_READ: s="SSL_ERROR_WANT_READ"; break;
			case SSL_ERROR_WANT_WRITE: s="SSL_ERROR_WANT_WRITE"; break;
			case SSL_ERROR_WANT_CONNECT: s="SSL_ERROR_WANT_CONNECT"; break;
			case SSL_ERROR_WANT_ACCEPT: s="SSL_ERROR_WANT_ACCEPT"; break;
			case SSL_ERROR_WANT_X509_LOOKUP: s="SSL_ERROR_WANT_X509_LOOKUP"; break;
			case SSL_ERROR_WANT_ASYNC: s="SSL_ERROR_WANT_ASYNC"; break;
			case SSL_ERROR_WANT_ASYNC_JOB: s="SSL_ERROR_WANT_ASYNC_JOB"; break;
			case SSL_ERROR_SYSCALL: s="SSL_ERROR_SYSCALL"; break;
			case SSL_ERROR_SSL: s="SSL_ERROR_SSL"; break;
		}
		fprintf(stderr, "SSL error: %s\n", s);
		ERR_print_errors_fp(stderr);
		return 3;
	}

	SSL_write(ssl, obuf, strlen(obuf));

	//open a file descriptor for writing the certificate to the certificates file
	string certificateFile = "./certificates/" + username + ".cert";

	FILE *certfp = fopen(certificateFile.c_str(), "wb");
	if (certfp == NULL)
	{
		cerr << "Unable to write to certificate file" << endl;
		return 1;
	}

	while ((ilen = SSL_read(ssl, ibuf, sizeof ibuf - 1)) > 0) {
		ibuf[ilen] = '\0';
		if (fwrite(ibuf, sizeof(char), ilen, certfp) < 0)
		{
			cerr << "Writing to file failed" << endl;
			return 1;
		}
		printf("%s", ibuf);
	}

	return 0;
}
