//Our includes
#include "EventFilter.h"
#include "ObjectFinder.h"

//Qt includes
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QQuickItem>
#include <QQuickView>
#include <QGuiApplication>

EventFilter::EventFilter(QObject *parent) : QObject(parent)
{
}

QString EventFilter::output() const
{
    QString outputData;
    QTextStream stream(&outputData);

    QHash<QString, QString> chainNameToObjName = [this]() {
        QHash<QString, QString> hash;
        int objectCount = 1;
        for(const auto& command : std::as_const(mCommands)) {
            if(!hash.contains(command.chainName())) {
                hash.insert(command.chainName(), QString("%1_obj%2").arg(command.mObject->objectName()).arg(objectCount));
                objectCount++;
            }
        }
        return hash;
    }();

    QSet<QString> objectPrinted;

    auto printMouse = [&](const Command& command) {

        auto first = static_cast<QMouseEvent*>(command.mEventList.first().data());
        auto last = static_cast<QMouseEvent*>(command.mEventList.last().data());

        auto isClick = [&]() {
            return first->globalPosition() == last->globalPosition();
        };

        auto objName = chainNameToObjName.value(command.chainName());

        if(!objectPrinted.contains(objName)) {
            stream << QString("let %1 = ObjectFinder.findObjectByChain(%2, \"%3\")\n")
                      .arg(objName, rootObjectId(), command.mObjectChainName.join("->"));
            objectPrinted.insert(objName);
        }

        auto firstPosition = first->position();
        if(isClick()) {
            stream << QString("mouseClick(%1, %2, %3)\n")
                      .arg(objName)
                      .arg(firstPosition.x())
                      .arg(firstPosition.y());
        } else {
            auto lastPosition = last->position();
            stream << QString("mousePress(%1, %2, %3)\n")
                      .arg(objName)
                      .arg(firstPosition.x())
                      .arg(firstPosition.y());
            stream << QString("mouseRelease(%1, %2, %3)\n")
                      .arg(objName)
                      .arg(lastPosition.x())
                      .arg(lastPosition.y());
        }
        // stream << QString("waitForRendering(%1)\n")
        //           .arg(objName);
    };

    auto printKey = [&](const Command& command) {
        auto first = static_cast<QKeyEvent*>(command.mEventList.first().data());

        if(first->key() >= Qt::Key_A
                && first->key() <= Qt::Key_Z)
        {
            stream << QString("keyClick(\"%1\")")
                      .arg(first->text());
        } else {
            stream << QString("keyClick(%1, %2) //%3")
                      .arg(first->key())
                      .arg((int)first->modifiers())
                      .arg(QKeySequence(first->key(), first->modifiers()).toString());
        }
    };

    auto printCommand = [&](const Command& command)
    {
        switch(command.mEventList.first()->type()) {
        case QEvent::MouseButtonPress:
            printMouse(command);
            break;
        case QEvent::MouseButtonRelease:
            Q_ASSERT(false); //The first event should never be a release this is a bug
            break;
        case QEvent::KeyPress:
            printKey(command);
            break;
        case QEvent::KeyRelease:
            Q_ASSERT(false); //The first event should never be a release this is a bug
            break;
        default:
            break;
        }

        stream << "\n";
    };

    for(const auto& command : std::as_const(mCommands)) {
        printCommand(command);
    }

    return outputData;
}

bool EventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if(!dynamic_cast<QQuickView*>(watched)
            && dynamic_cast<QQuickItem*>(watched)
            && isChildOfIgnored(static_cast<QQuickItem*>(watched))) {
        switch(event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease: {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            qDebug() << "QObject:" << watched << "Name:" << watched->objectName() << "Chain:" << ObjectFinder::toChainString(watched) << event->type() << mouseEvent->pos() << mouseEvent->globalPosition() << mouseEvent->isAccepted();
            addCommand(dynamic_cast<QQuickItem*>(watched), event);
            break;
        }
        case QEvent::KeyPress:
        case QEvent::KeyRelease: {
            auto keyEvent = static_cast<QKeyEvent*>(event);
            qDebug() << "QObject:" << watched << "Name:" << watched->objectName() << event->type() << keyEvent->key() ;
            addCommand(dynamic_cast<QQuickItem*>(watched), event);
            break;
        }
        default:
            break;
        }
    }

    return QObject::eventFilter(watched, event);
}

void EventFilter::addCommand(QQuickItem* object, QEvent *event)
{
    auto isModifier = [](QEvent* event) {
        Q_ASSERT(dynamic_cast<QKeyEvent*>(event));
        auto keyEvent = static_cast<QKeyEvent*>(event);
        switch(keyEvent->key()) {
        case Qt::Key_Shift:
        case Qt::Key_Control:
        case Qt::Key_Alt:
            return true;
        default:
            break;
        }

        return false;
    };

    switch(event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        break;
    case QEvent::KeyPress:
        if(isModifier(event)) { return; } //Ignore modifiers
        break;
    case QEvent::KeyRelease:
        if(isModifier(event)) { return; } //Ignore modifiers
        break;
    default:
        return;
    }

    auto emitCommandAdded = [this]() {
        auto& lastCommand = mCommands.last();
        auto lastEvent = lastCommand.mEventList.last();
        switch(lastEvent->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease: {
            auto mouseEvent = static_cast<QMouseEvent*>(lastEvent.data());
            ObjectFinder finder;
            auto foundItem = finder.findObjectByChain(mRootObject, lastCommand.chainName());
            emit clicked(foundItem, mouseEvent->pos());
            }
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            break;
        default:
            return;
        }

        emit outputChanged();
    };


    if(!mCommands.isEmpty()) {
        auto& lastCommand = mCommands.last();
        qDebug() << "Can add:" << event << lastCommand.canAdd(object, event);
        if(lastCommand.canAdd(object, event)) {
            lastCommand.add(event);
            lastCommand.updateFirstEvent(object, event);
            emitCommandAdded();
            return;
        }
    }

    //New command
    mCommands.append(Command(object, event));
    emitCommandAdded();
}

bool EventFilter::isChildOfIgnored(QQuickItem *object) const
{
    auto parent = object;
    while(parent != nullptr) {
        if(mIgnoreItems.contains(parent)) {
            return false;
        }
        parent = parent->parentItem();
    }
    return true;

}

EventFilter::Command::Command(QQuickItem *object, QEvent *event) :
    mObject(object),
    mObjectChainName(ObjectFinder::toChain(object)),
    mEventList({cloneEvent(event)})
{
}

EventFilter::Command::~Command()
{
}

bool EventFilter::Command::canAdd(QQuickItem *object, QEvent *event)
{
    Q_ASSERT(!mEventList.isEmpty());
    Q_ASSERT(object);

    auto lastEvent = mEventList.last();
    if(lastEvent->type() == event->type()) {
        return true;
    }

    switch(lastEvent->type()) {
    case QEvent::MouseButtonPress:
        return event->type() == QEvent::MouseButtonRelease;
    case QEvent::MouseButtonRelease:
        return false;
    case QEvent::KeyPress:
        return event->type() == QEvent::KeyRelease;
    case QEvent::KeyRelease:
        return false;
    default:
        Q_ASSERT("Unsupport event type");
    }

    return false;
}

void EventFilter::Command::updateFirstEvent(QQuickItem *object, QEvent* event)
{
    auto newChain = ObjectFinder::toChain(object);
    if(newChain.size() > mObjectChainName.size()) {
        auto& firstEvent = mEventList.first();

        if(firstEvent->type() == QEvent::MouseButtonPress
                && event->type() == QEvent::MouseButtonPress) {
            mObjectChainName = newChain;
            mObject = object;
            auto firstMouseEvent = static_cast<QMouseEvent*>(firstEvent.data());
            auto newMouseEvent = static_cast<QMouseEvent*>(event);
            mEventList.first().reset(new QMouseEvent(newMouseEvent->type(),
                                                     newMouseEvent->pos(),
                                                     newMouseEvent->globalPosition(),
                                                     newMouseEvent->button(),
                                                     newMouseEvent->buttons(),
                                                     newMouseEvent->modifiers()));
        }
    }
}

void EventFilter::Command::add(QEvent *event)
{
    mEventList.append(cloneEvent(event));
}

QString EventFilter::Command::chainName() const
{
    return ObjectFinder::toChainString(mObjectChainName);
}


QSharedPointer<QEvent> EventFilter::Command::cloneEvent(QEvent *event)
{
    if(!event) {
        return nullptr;
    }

    typedef QSharedPointer<QEvent> QEventPtr;

    switch(event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease: {
        auto  mouseEvent = static_cast<QMouseEvent*>(event);
        return QEventPtr(new QMouseEvent(mouseEvent->type(),
                               mObject->mapFromGlobal(mouseEvent->globalPosition()),
                               mouseEvent->scenePosition(),
                               mouseEvent->globalPosition(),
                               mouseEvent->button(),
                               mouseEvent->buttons(),
                               mouseEvent->modifiers(),
                               mouseEvent->source()));
    }
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
        auto keyEvent = static_cast<QKeyEvent*>(event);
        return QEventPtr(new QKeyEvent(keyEvent->type(),
                                       keyEvent->key(),
                                       keyEvent->modifiers(),
                                       keyEvent->text()));
    }
    default:
        break;

    }
    return nullptr;
}

void EventFilter::setRootObjectId(QString rootObjectId) {
    if(mRootObjectId != rootObjectId) {
        mRootObjectId = rootObjectId;
        emit rootObjectIdChanged();
    }
}

void EventFilter::setRootObject(QQuickItem* rootObject) {
    if(mRootObject != rootObject) {
        if(mRootObject) {
            QGuiApplication::instance()->removeEventFilter(this);
//            mRootObject->removeEventFilter(this);
        }

        mRootObject = rootObject;

        if(mRootObject) {
            QGuiApplication::instance()->installEventFilter(this);
//            mRootObject->installEventFilter(this);
        }

        emit rootObjectChanged();
    }
}

void EventFilter::addIgnore(QQuickItem *item)
{
    mIgnoreItems.insert(item);
}

void EventFilter::clear()
{
    mCommands.clear();
    emit outputChanged();
}

