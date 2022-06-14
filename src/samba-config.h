#ifndef SAMBACONFIG_H
#define SAMBACONFIG_H

#include <QObject>

class SambaConfigPrivate;

class SambaConfig : public QObject
{
    Q_OBJECT
    Q_CLASSINFO ("D-Bus Interface", DBUS_NAME)
public:
    const static SambaConfig* getInstance ();

private:
    SambaConfig(SambaConfig&){};
    ~SambaConfig(){};
    explicit SambaConfig(QObject *parent = nullptr);

public Q_SLOTS:
    bool init (QString name, int pid, int uid);
    void finished ();

    bool launchSmbd();
    bool isSmbdLaunched ();
    bool userIsInSambaGroup ();
    bool addUserInGroup ();
    bool hasPasswd ();
    bool setPasswd (QString passwd);

Q_SIGNALS:

private:
    SambaConfigPrivate*         d_ptr;
    Q_DECLARE_PRIVATE(SambaConfig)
};

#endif // SAMBACONFIG_H
