#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <QObject>
#include <QStringList>
#include <QPointer>
#include <QQuickItem>
#include <QQmlEngine>

class EventFilter : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString rootObjectId READ rootObjectId WRITE setRootObjectId NOTIFY rootObjectIdChanged)
    Q_PROPERTY(QQuickItem* rootObject READ rootObject WRITE setRootObject NOTIFY rootObjectChanged)
    Q_PROPERTY(QString output READ output NOTIFY outputChanged)

public:
    EventFilter(QObject* parent = nullptr);

    QString output() const;

    QString rootObjectId() const;
    void setRootObjectId(QString rootObjectId);

    QQuickItem* rootObject() const;
    void setRootObject(QQuickItem* rootObject);

    Q_INVOKABLE void addIgnore(QQuickItem* item);
    Q_INVOKABLE void clear();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

    void addCommand(QQuickItem *object, QEvent* event);

private:
    class Command {
    public:
        Command() = default;
        Command(QQuickItem* object, QEvent* event);
        ~Command();

        QPointer<QQuickItem> mObject;
        QStringList mObjectChainName;
        QVector<QSharedPointer<QEvent>> mEventList;

        bool canAdd(QQuickItem* object, QEvent* event);
        void updateFirstEvent(QQuickItem* object, QEvent* event);
        void add(QEvent* event);

        QString chainName() const;

    private:
        QSharedPointer<QEvent> cloneEvent(QEvent* event);

    };

    QVector<Command> mCommands;
    QString mRootObjectId; //!<
    QPointer<QQuickItem> mRootObject; //!<
    QSet<QQuickItem*> mIgnoreItems;

    bool isChildOfIgnored(QQuickItem *object) const;

signals:
    void rootObjectIdChanged();
    void rootObjectChanged();
    void outputChanged();

    void clicked(QQuickItem* item, QPoint point);

};

inline QString EventFilter::rootObjectId() const {
    return mRootObjectId;
}

inline QQuickItem* EventFilter::rootObject() const {
    return mRootObject;
}

#endif // EVENTFILTER_H
