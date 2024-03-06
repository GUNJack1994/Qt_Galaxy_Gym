#ifndef QUERYTHREAD_H
#define QUERYTHREAD_H
#include <QObject>
#include <QSqlDatabase>
//#include <QHash>
#include <QPixmap>
#include <QVariant>

typedef QHash<int, QPixmap> PixHash;
typedef QHash<int, QVariant> VHash;
typedef QList< VHash > VariantHash;

Q_DECLARE_METATYPE(PixHash)
Q_DECLARE_METATYPE(VHash)
Q_DECLARE_METATYPE(VariantHash)

class QueryThread : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool lock READ lock WRITE setLock NOTIFY lockChanged)
public:
    QueryThread();
    bool isReady();
    void abort();
    bool lock() const;

public slots:
    void getPhotoList(const QString &params = "");
    void getUserList(const QString &params = "");
    void setLock(bool lock);

private:
    QSqlDatabase m_db;
    bool m_abort;
    bool m_lock;
    bool checkConnection();

signals:
    void photoListReady(PixHash);
    void userListReady(VariantHash);
    void lockChanged(bool lock);
};

#endif // QUERYTHREAD_H
