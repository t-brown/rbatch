/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Timothy Brown
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

#include "common.h"

/**
 *  A wrapper around `getenv()` that errors out if it can not 
 *  retreive the environment variable.
 **/
char *
get_env(const char *key)
{
	char *value = NULL;

	value = getenv(key);
	if (!value) {
		errx(EX_UNAVAILABLE, "unable to retrieve %s", key);
	}
#ifdef DEBUG
	printf("%s: %s\n", key, value);
#endif
	return value;
}

/**
 * Wrapper to return either an IPv4 or IPv6 address.
 **/
char *
get_ip(struct sockaddr *sa)
{
	struct sockaddr_in *sin = NULL;
	struct sockaddr_in6 *sin6 = NULL;
	char *ip = NULL;

	switch (sa->sa_family) {
		case AF_INET:
			sin = (struct sockaddr_in *)sa;
			ip = malloc(INET_ADDRSTRLEN);
			inet_ntop(AF_INET, &(sin->sin_addr), ip, INET_ADDRSTRLEN);
		case AF_INET6:
			sin6 = (struct sockaddr_in6 *)sa;
			ip = malloc(INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, &(sin6->sin6_addr), ip, INET6_ADDRSTRLEN);
		case AF_LOCAL:
			break;
		default:
			break;
	}
	return ip;

}

/**
 * Get a connection.
 **/
int
get_conn(const char *host, int port, int client, struct addrinfo **p, int *sockfd)
{
	int rv = 0;
	int one = 1;
	struct addrinfo hints = {0};
        struct addrinfo *servinfo = NULL;
	char strp[128] = {0};
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; /* AF_UNSPEC; */
	hints.ai_socktype = SOCK_STREAM;
	if (!client) {
		hints.ai_flags = AI_PASSIVE;
	}

	snprintf(strp, sizeof(strp), "%d", port);
#if DEBUG
	printf("host: %s port: %s\n", host, strp);
#endif
        if ((rv = getaddrinfo(host, strp, &hints, &servinfo)) != 0) {
		err(EX_UNAVAILABLE, "getaddrinfo: %s\n", gai_strerror(rv));
	}

	for(*p = servinfo; *p != NULL; *p = (*p)->ai_next) {
		if ((*sockfd = socket((*p)->ai_family, (*p)->ai_socktype, (*p)->ai_protocol)) == -1) {
			warnx("unable to create a socket");
			continue;
		}

		if (client) {
			if (connect(*sockfd, (*p)->ai_addr, (*p)->ai_addrlen) == -1) {
				close(*sockfd);
				warn("unable to connect");
				continue;
			}
		} else {
			if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1) {
				err(EX_UNAVAILABLE, "unable to set reuse");
			}
			if (bind(*sockfd, (*p)->ai_addr, (*p)->ai_addrlen) == -1) {
				close(*sockfd);
				warn("unable to bind");
				continue;
			}
		}

		break;
	}
	freeaddrinfo(servinfo);
	return EXIT_SUCCESS;
}

/**
 * Parse the command line arguments.
 **/
int
parse_args(int argc, char **argv)
{
	int opt = 0;
	int opt_index = 0;
	char msg[128] = {0};
	char *soptions = "Hp:v";         /* short options structure */
	static struct option loptions[] = {     /* long options structure */
		{"help",       no_argument,        NULL,  'H'},
		{"port",       required_argument,  NULL,  'p'},
		{"version",    no_argument,        NULL,  'V'},
		{NULL,         1,                  NULL,  0}
	};

	/* Parse command line */
	while ((opt = getopt_long(argc, argv, soptions, loptions,
			  &opt_index)) != -1) {
		switch (opt) {
		case 'H':
			print_help(NULL);
			break;
		case 'p':
			options.port = strtol(optarg, NULL, 10);
			break;
		case 'V':
			print_version();
			break;
		default:
			snprintf(msg, sizeof(msg), "Unknown option: %s", optarg);
			print_help(msg);
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc >= 1) {
		if (options.hostfile) {
			free(options.hostfile);
		}
		options.hostfile = strdup(argv[0]);
	}

	return EXIT_SUCCESS;
}

/**
 * Prints a short program usage statement, explaining the
 * command line arguments and flags expected.
 **/
void
print_help(const char *msg)
{
	printf("\
%s\n\
usage: %s [-h] [-p #] [-v]\n\
  -H, --help         Display this help and exit.\n\
  -p, --port  port   Port number to contact the server on.\n\
  -V, --version      Print the program version number.\n\
", msg, program_invocation_short_name);
	exit(EXIT_FAILURE);
}

/**
 * Prints the program name and version.
 **/
void
print_version(void)
{
	printf("%s: RBATCH_VERSION\n", program_invocation_short_name);
	exit(EXIT_FAILURE);
}
