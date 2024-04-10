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

#define RBATCH_VERSION "0.1"
#define RBATCH_PORT    2323
#define RBATCH_BACKLOG 128
#define RBATCH_MAX_MSG 256

struct rbatch_info {
	uint32_t nslots;
	uint32_t jobid;
};

struct opts {
	int port;
	char *hostfile;
};

extern struct opts options;

/** Wrapper around getenv with warnings. **/
char *get_env(const char *name);

/** Wrapper around sockaddr to get the address. **/
char *get_ip(struct sockaddr *);

/** Wraper acound connect. **/
int get_conn(const char *, int, int, struct addrinfo **, int *);

/** Parse command line arguments. **/
int parse_args(int, char **);

/** Help/Usage statement. **/
void print_help(const char *);

/** Version information. **/
void print_version(void);
