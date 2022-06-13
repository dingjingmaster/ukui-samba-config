#include "polkit-helper.h"

#include <gio/gio.h>

#include <unistd.h>

#include <QDebug>

int main (int argc, char* argv[])
{
    PolkitHelper* p = new PolkitHelper;

    g_autoptr(GError) error = nullptr;

    GDBusConnection* conn = g_bus_get_sync (G_BUS_TYPE_SYSTEM, nullptr, &error);
    if (error) {
        qWarning() << error->message;
        return -1;
    }

//    g_dbus_connection_call (conn, );



    qDebug() << p->checkAuthorization("org.ukui.samba.config.authorization", getpid(), getuid());

    return 0;
}
