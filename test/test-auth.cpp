#include <gio/gio.h>
#include <unistd.h>

#include <QDebug>

#include "../src/samba-config-interface.h"
#include <QDebug>

int main (int argc, char* argv[])
{
    SambaConfigInterface si(DBUS_NAME, DBUS_PATH, QDBusConnection::systemBus());


    if (si.init(g_get_user_name(), getpid(), getuid())) {
        if (!si.userIsInSambaGroup()) {
            si.addUserInGroup();
        }

        if (!si.hasPasswd()) {
            si.setPasswd("qwer1234");
        }
    }




    return 0;
}
