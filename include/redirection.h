#ifndef REDIRECTION_H
#define REDIRECTION_H

#define REDIRECT_SUCCESS 1
#define REDIRECT_FAILURE 0

int redirect_in(int argc, char* argv[]);
int redirect_out(int argc, char* argv[]);
int redirect_pipe(int argc, char* argv[]);

#endif
