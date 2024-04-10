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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

struct opts options = {0};              /**< Program options */


int
main(int argc, char **argv)
{
	int sockfd = 0;
        uint32_t omp = 0;
	struct addrinfo *p = NULL;
	struct rbatch_info msg = {0};
	char *id = NULL;
	char *server = NULL;

	options.port = RBATCH_PORT;
	parse_args(argc, argv);

	omp = strtol(get_env("OMP_NUM_THREADS"), NULL, 10);
	msg.nslots = sysconf(_SC_NPROCESSORS_ONLN);
	if (omp != 0) {
		msg.nslots = msg.nslots/omp;
	}
	msg.nslots = htonl(msg.nslots);

	id = get_env("AWS_BATCH_JOB_ID");
	msg.jobid = htonl(strtol(id, NULL, 10));

	server = get_env("AWS_BATCH_JOB_MAIN_NODE_PRIVATE_IPV4_ADDRESS");
#if DEBUG
	printf("Server: %s\n", server);
#endif

	if (get_conn(server, options.port, 1, &p, &sockfd) != 0) {
		errx(EX_UNAVAILABLE, "failed to connect");
	}

	if (send(sockfd, &msg, sizeof(msg), 0) == -1) {
		err(EX_UNAVAILABLE, "unable to send message");
	}

	close(sockfd);

	return EXIT_SUCCESS;
}

