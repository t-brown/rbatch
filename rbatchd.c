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

#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

struct opts options = {0};              /**< Program options */

void
sigchld_handler(int s)
{
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}


int
main(int argc, char **argv)
{
	int sockfd = 0;
	int nfd = 0;
	int rv = 0;
	struct addrinfo *p = NULL;
	struct sockaddr_storage caddr = {0};
	socklen_t sin_size = 0;
	struct sigaction sa = {0};
	char *s = NULL;
	FILE *f = NULL;

	options.port = RBATCH_PORT;
	options.hostfile = strdup("hostfile");
	parse_args(argc, argv);

	if (get_conn(NULL, options.port, 0, &p, &sockfd) != 0) {
		errx(EX_UNAVAILABLE, "failed to connect");
	}

	if (p == NULL)  {
		errx(EX_UNAVAILABLE, "failed to connect");
	}

	if (listen(sockfd, RBATCH_BACKLOG) == -1) {
		errx(EX_UNAVAILABLE, "failed to listen");
	}

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		errx(EX_UNAVAILABLE, "sig action");
	}

	while(1) {
		sin_size = sizeof caddr;
		nfd = accept(sockfd, (struct sockaddr *)&caddr, &sin_size);
		if (nfd == -1) {
			warn("accept");
			continue;
		}
		s = get_ip((struct sockaddr *)&caddr);
		if (!fork()) {
			struct rbatch_info msg = {0};
			close(sockfd);
			if (recv(nfd, &msg, sizeof(msg), 0) == -1) {
				err(EX_UNAVAILABLE, "recv");
			}
			f = fopen(options.hostfile, "a");
			if (!f) {
				exit(0);
			}
			fprintf(f, "%s slots=%d\n", s, ntohl(msg.nslots));
			fclose(f);
			exit(0);
		}
		close(nfd);
		if (s) {
			free(s);
		}
	}

	return EXIT_SUCCESS;
}

