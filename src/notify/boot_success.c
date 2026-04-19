#include <stdio.h>
#include <unistd.h>

int main(void)
{
    execl("/usr/sbin/fw_setenv", "fw_setenv", "upgrade_available", "0", (char *)NULL);
    execl("/usr/bin/fw_setenv", "fw_setenv", "upgrade_available", "0", (char *)NULL);
    execlp("fw_setenv", "fw_setenv", "upgrade_available", "0", (char *)NULL);

    perror("fw_setenv");
    return 127;
}
