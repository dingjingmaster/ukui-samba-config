#include "samba-config.h"
#include <QMutex>
#include <glib.h>
#include <QCoreApplication>
#include <QProcess>

#include <polkit/polkit.h>

#include <QDebug>

class SambaConfigPrivate
{
public:
    explicit SambaConfigPrivate (SambaConfig* sm);

    bool launchSmbd();
    bool smbdIsActive ();
    bool userInSamba ();
    bool userInSambaGroup();
    bool addUserToSambaGroup();
    bool setUserPasswd(const QString& pass);

    bool checkAuthorization ();

public:
    SambaConfig*        q_ptr;

    QString             mUserName;
    int                 mUserUid;
    int                 mUserPid;
    PolkitAuthority*    mAuth = nullptr;

    QMutex              mLock;
};

SambaConfigPrivate::SambaConfigPrivate(SambaConfig *sm)
    : q_ptr(sm), mUserUid(-1), mUserPid(-1)
{
    g_autoptr(GError) error = NULL;

    mAuth = polkit_authority_get_sync(NULL, &error);
    if (error) {
        qWarning() << error->message;
    }
}

bool SambaConfigPrivate::launchSmbd()
{
    if (!QProcess::execute("systemctl", QStringList()<<"start"<<"smbd.service")) {
        return QProcess::execute("systemctl", QStringList()<<"enable"<<"smbd.service") ? false : true;
    }

    return false;
}

bool SambaConfigPrivate::smbdIsActive()
{
    return QProcess::execute("systemctl", QStringList()<<"status"<<"smbd") ? false : true;
}

bool SambaConfigPrivate::userInSamba()
{
    if (mUserName.isEmpty() || -1 == mUserUid) {
        return false;
    }

    return QProcess::execute("smbpasswd", QStringList()<<"-e"<<mUserName) ? false : true;
}

bool SambaConfigPrivate::userInSambaGroup()
{
    if (mUserName.isEmpty() || -1 == mUserUid) {
        return false;
    }

    return QProcess::execute("groups", QStringList()<<mUserName << "|" << "grep" << "sambashare") ? false : true;
}

bool SambaConfigPrivate::addUserToSambaGroup()
{
    if (mUserName.isEmpty() || -1 == mUserUid) {
        return false;
    }

    return QProcess::execute("usermod", QStringList() << "-G" << "sambashare" << "-a" << mUserName) ? false : true;
}

bool SambaConfigPrivate::setUserPasswd(const QString &pass)
{
    if (mUserName.isEmpty() || -1 == mUserUid) {
        return false;
    }

    qDebug() << "username:" << mUserName << " change passwd!";

//    const QString pwd = QString("%1\n").arg(pass);
//    const QString cmd = QString (""
//                                 "spawn smbpasswd -a %1;"
//                                 "expect \"*New SMB password*\"; send %2;"
//                                 "expect \"*Retype new SMB password*\"; send %3;"
//                                 "expect eof; exit"
//                                 "").arg(mUserName).arg(pwd).arg(pwd);

    g_autofree gchar* cmd = g_strdup_printf ("spawn smbpasswd -a \"%s\";"
                                             "expect \"*New SMB password*\"; send \"%s\\n\";"
                                             "expect \"*Retype new SMB password*\"; send \"%s\\n\";"
                                             "expect eof; exit", mUserName.toUtf8().constData(),
                                             pass.toUtf8().constData(), pass.toUtf8().constData());


    qDebug() << cmd;
    QProcess::execute("expect", QStringList() << "-c" << cmd);


    return userInSamba();
}

bool SambaConfigPrivate::checkAuthorization()
{
    bool ret = false;

    g_autoptr(GError) error = nullptr;

    PolkitSubject* proj = polkit_unix_process_new_for_owner (mUserPid, 0, mUserUid);

    PolkitAuthorizationResult* res = polkit_authority_check_authorization_sync (mAuth, proj, "org.ukui.samba.share.config.authorization", NULL,
                                                                                          POLKIT_CHECK_AUTHORIZATION_FLAGS_ALLOW_USER_INTERACTION, nullptr, &error);
    if (error) {
        qWarning() << error->message;
        goto out;
    }

    if (polkit_authorization_result_get_is_authorized(res)) {
        ret = true;
    }

out:
    if (proj)       g_object_unref (proj);
    if (res)        g_object_unref (res);

    return ret;
}

const SambaConfig *SambaConfig::getInstance()
{
    static SambaConfig sc;

    return &sc;
}

SambaConfig::SambaConfig(QObject *parent)
    : QObject{parent}, d_ptr (new SambaConfigPrivate(this))
{

}

bool SambaConfig::init(QString name, int pid, int uid)
{
    Q_D (SambaConfig);

    if (d->mLock.tryLock(300)) {
        d->mUserName = name;
        d->mUserPid = pid;
        d->mUserUid = uid;

        return true;
    }

    return false;
}

void SambaConfig::finished()
{
    Q_D (SambaConfig);
    d->mLock.unlock();
    d->mUserName = "";
    d->mUserPid = -1;
    d->mUserUid = -1;

    QCoreApplication::exit(0);
}

bool SambaConfig::launchSmbd()
{
    Q_D (SambaConfig);

    if (d->checkAuthorization()) {
        return d->launchSmbd();
    }

    return false;
}

bool SambaConfig::isSmbdLaunched()
{
    Q_D (SambaConfig);

    return d->smbdIsActive();
}

bool SambaConfig::userIsInSambaGroup()
{
    Q_D (SambaConfig);

    return d->userInSambaGroup();
}

bool SambaConfig::addUserInGroup()
{
    Q_D (SambaConfig);

    if (d->checkAuthorization()) {
        return d->addUserToSambaGroup();
    }

    return false;
}

bool SambaConfig::hasPasswd()
{
    Q_D (SambaConfig);

    if (d->checkAuthorization()) {
        return d->userInSamba();
    }

    return false;
}

bool SambaConfig::setPasswd(QString passwd)
{
    Q_D (SambaConfig);

    if (d->checkAuthorization()) {
        return d->setUserPasswd(passwd);
    }

    return false;
}


