#ifndef POLKITHELPER_H
#define POLKITHELPER_H

#include <QObject>

class PolkitHelperPrivate;

class PolkitHelper : QObject
{
    Q_OBJECT
public:
    explicit PolkitHelper(QObject* parent=nullptr);

    bool checkAuthorization (const QString& actionID, qint64 pid, qint32 uid);

private:
    PolkitHelper(const QObject&)=delete;

private:
    PolkitHelperPrivate*        d_ptr;

    Q_DISABLE_COPY(PolkitHelper)
    Q_DECLARE_PRIVATE(PolkitHelper)
};


#endif // POLKITHELPER_H
