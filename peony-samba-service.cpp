#include <QCoreApplication>

#include <gio/gio.h>
#include "samba-config.h"

#include <QDebug>

static GDBusObjectManagerServer* manager = NULL;

static void on_bus_acquired (GDBusConnection* conn, const char* name, gpointer udata);

int main (int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    guint id = g_bus_own_name (G_BUS_TYPE_SESSION, DBUS_NAME, G_BUS_NAME_OWNER_FLAGS_NONE, on_bus_acquired, NULL, NULL, NULL, NULL);

    int ret = app.exec();

    g_bus_unwatch_name(id);

    return ret;
}


static void on_bus_acquired (GDBusConnection* conn, const char* name, gpointer udata)
{
    manager = g_dbus_object_manager_server_new (DBUS_PATH);
    if (manager) {
        g_autoptr(SambaConfigSkeleton) smbConfig = samba_config_skeleton_new(DBUS_PATH "/config");
        if (G_IS_DBUS_OBJECT_SKELETON(smbConfig)) {
            g_dbus_object_manager_server_export (manager, G_DBUS_OBJECT_SKELETON(smbConfig));
        } else {
            qWarning() << "G_IS_DBUS_OBJECT_SKELETON(smbConfig): false!";
        }
    }

    g_dbus_object_manager_server_set_connection (manager, conn);
}
