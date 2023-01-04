//
// Created by Vasco on 10/31/2022.
//

#ifndef SO_2022_ISEC_BACKEND_H
#define SO_2022_ISEC_BACKEND_H
#include "communication.h"
#include "users_lib.h"
#include <wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define DEFAULT_MAXUSERS 10
#define DEFAULT_LEILAODIR "."

#define SERVER_FIFO "tmp/dict_fifo"
#define CLIENT_FIFO "tmp/resp%d_fifo"
#define TAM_MAX 100

#endif //SO_2022_ISEC_BACKEND_H
