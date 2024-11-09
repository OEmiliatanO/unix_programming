#ifndef __MYSIG_H__
#define __MYSIG_H__
typedef void (*sighandler_t)(int);
sighandler_t mysigset(int sig, sighandler_t handler);
int mysighold(int sig);
int mysigrelse(int sig);
int mysigignore(int sig);
int mysigpause(int sig);
#endif
