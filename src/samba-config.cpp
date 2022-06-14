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

    QProcess::execute ("smbpasswd", QStringList() << "-x" << mUserName);

    const QString pwd = QString("send \"%1\n;\"").arg(pass);

    return QProcess::execute("spawn", QStringList() << "-c" << "spawn" << "smpasswd" << "-a" << mUserName << "; expect"
                             << "\"*New SMB password*\"" << pwd
                             << "\"*Retype new SMB password*\"" << pwd
                             << "expect eof; exit") ? false : true;
}

bool SambaConfigPrivate::checkAuthorization()
{
    g_autoptr(GError) error = nullptr;

    g_autoptr(PolkitSubject) proj = polkit_unix_process_new_for_owner (mUserPid, 0, mUserUid);

    g_autoptr(PolkitAuthorizationResult) res = polkit_authority_check_authorization_sync (mAuth, proj, "org.ukui.samba.share.config.authorization", NULL,
                                                                                          POLKIT_CHECK_AUTHORIZATION_FLAGS_ALLOW_USER_INTERACTION, nullptr, &error);
    if (error) {
        qWarning() << error->message;
        goto out;
    }

    if (polkit_authorization_result_get_is_authorized(res)) {
        return true;
    }

out:
    return false;
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


