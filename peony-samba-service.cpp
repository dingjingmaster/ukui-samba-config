#include <QCoreApplication>

#include <gio/gio.h>
#include "src/samba-config.h"

#include <QDebug>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

static GDBusObjectManagerServer* manager = NULL;

static bool registManager (SambaConfig& sc);

int main (int argc, char* argv[])
{
    int ret = -1;
    QCoreApplication app(argc, argv);

    SambaConfig* sc = const_cast<SambaConfig*>(SambaConfig::getInstance());
    if (registManager(*sc)) {
        ret = app.exec();
    }

    return ret;
}


static bool registManager (SambaConfig& sc)
{
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.interface()->isServiceRegistered(DBUS_NAME)) {
        qWarning() << "dbus: " DBUS_NAME " already regist!";
        return false;
    }

    if (!bus.registerService(DBUS_NAME)) {
        qWarning() << "dbus: " DBUS_NAME " regist failed!";
        return false;
    }

    if (!bus.registerObject(DBUS_PATH, DBUS_NAME, static_cast<QObject*>(&sc), QDBusConnection::ExportNonScriptableSlots|QDBusConnection::ExportNonScriptableSignals)) {
        qWarning() << "dbus: " DBUS_NAME " regist object failed!";
        return false;
    }

    qDebug() << "dbus: " DBUS_NAME " regist successed!";

    return true;
}
