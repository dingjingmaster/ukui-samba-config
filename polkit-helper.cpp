#include "polkit-helper.h"

#include "global.h"

#include <polkit/polkit.h>

#include <QDebug>

class PolkitHelperPrivate
{
public:
    explicit PolkitHelperPrivate (PolkitHelper* priv);
    PolkitHelperPrivate(PolkitHelperPrivate&)=delete;
    ~PolkitHelperPrivate();

public:
    PolkitAuthority*                mAuth = nullptr;
    const PolkitAuthority*          q_ptr;
};

PolkitHelperPrivate::PolkitHelperPrivate(PolkitHelper *priv)
{
    g_autoptr(GError) error = NULL;

    mAuth = polkit_authority_get_sync(NULL, &error);
    if (error) {
        qWarning() << error->message;
    }
}

bool PolkitHelper::checkAuthorization(const QString& actionID, qint64 pid, qint32 uid)
{
    Q_D(PolkitHelper);

    g_autoptr(GError) error = nullptr;

    g_autoptr(PolkitSubject) proj = polkit_unix_process_new_for_owner (pid, 0, uid);

    g_autoptr(PolkitAuthorizationResult) res = polkit_authority_check_authorization_sync (d->mAuth, proj, actionID.toUtf8().constData(), NULL,
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

PolkitHelper::PolkitHelper(QObject* obj) : QObject(obj),
    d_ptr (new PolkitHelperPrivate(this))
{

}


