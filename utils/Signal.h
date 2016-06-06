//
// Created by root on 3/16/16.
//

#ifndef YFQSERVER_SIGNAL_H
#define YFQSERVER_SIGNAL_H

#include <signal.h>

namespace ft {
    typedef void (*Sigfunc)(int);

    /**
     * register a handler($func) for a specified signal($signo)
     * Return: SIG_ERR on failed, $func on success
     */
    Sigfunc signal(int signo, Sigfunc func) {
        struct sigaction act, oact;

        act.sa_handler = func;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        if (signo == SIGALRM) {
#ifdef  SA_INTERRUPT
            act.sa_flags |= SA_INTERRUPT;           /* SunOS 4.x */
#endif
        } else {
#ifdef  SA_RESTART
            act.sa_flags |= SA_RESTART;             /* SVR4, 44BSD */
#endif
        }

        if (sigaction(signo, &act, &oact) < 0) {
            return (SIG_ERR);
        }

        return (oact.sa_handler);
    }
}
#endif //YFQSERVER_SIGNAL_H
