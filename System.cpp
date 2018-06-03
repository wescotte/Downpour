#include "System.h"

void Error(char *errmsg)
{
    MessageBox (NULL, errmsg, "Error", MB_ICONHAND);
    return;
}

void Status(char *msg)
{
    MessageBox(NULL, msg, "Status", MB_OK);
    return;
}
