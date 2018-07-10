#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
// This one's got the posix_openpt flags in it //
#include <fcntl.h>
// This has PATH_MAX //
#include <sys/param.h>
#include <stdbool.h>
#include <unistd.h>

#define SHELL "/usr/local/bin/dash"

struct PTY {
  int master, agent;
};

bool create_pty(struct PTY *pty) {
  char masterpath[PATH_MAX];
  char *agent_name;
  pty->master = posix_openpt(O_RDWR | O_NOCTTY);
  if (pty->master == -1) {
    perror("posix_openpt");
  }
  if (grantpt(pty->master) == -1) {
    perror("grantpt");
  }
  if (unlockpt(pty->master) == -1) {
    perror("unlockpt");
  }
  if(fcntl(pty->master, F_GETPATH, masterpath) != 1){
    printf("Opened pty master device: %s\n", masterpath);
  }
  agent_name = ptsname(pty->master);
  if (agent_name == NULL) {
    perror("ptsname");
    return false;
  }
  pty->agent = open(agent_name, O_RDWR | O_NOCTTY);
  if(pty->agent == -1) {
    perror("open(agent_name)");
  }
  printf("Opened pty agent device: %s\n", agent_name);
  return true;
}

bool spawn(struct PTY *pty){
  pid_t p;
  char **env = { NULL };

  p = fork();
  if (p == 0){
    // We are the child process //
    close(pty->master);
    // So we can make our process the head of the process group //
    setsid();
    if (ioctl(pty->agent, TIOCSCTTY, NULL) == -1){
      perror("ioctl(TIOCSCTTY)");
      return false;
    }

    dup2(pty->agent, 0); //stdin//
    dup2(pty->agent, 1); //stout//
    dup2(pty->agent, 2); //sterr//

    execle(SHELL, "-" SHELL, NULL, env);
    return false;
  } else if (p > 0) {
    // We are the parent process //
    close(pty->agent);
    return true;
  }
  return true;
}

int main(int argc, char **argv) {
  struct PTY pty;
  if (!create_pty(&pty))
    return 1;
  if (!spawn(&pty))
    return 1;
  return 0;
}
