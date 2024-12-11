#ifndef OBJECTFINDER_H
#define OBJECTFINDER_H

#include <QObject>
#include <QQuickItem>

class ObjectFinder : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit ObjectFinder(QObject *parent = nullptr);

    Q_INVOKABLE QQuickItem* findObjectByChain(QQuickItem* parent, const QString& chainName) const;

    static QStringList toChain(const QObject *object);
    static QString toChainString(const QStringList& chain);
    static QString toChainString(const QObject* object);

signals:

};

#endif // OBJECTFINDER_H
