#include "ObjectFinder.h"

const static QString seperator = QStringLiteral("->");

ObjectFinder::ObjectFinder(QObject *parent) : QObject(parent)
{

}

QQuickItem *ObjectFinder::findObjectByChain(QQuickItem *parent, const QString &chainName) const
{
    if(chainName.isEmpty()) {
        return nullptr;
    }

    QStringList chain = chainName.split(seperator);

    auto findChildren = [](QObject* parent, const QString& childName) {

        QSet<QObject*> items;

        auto addItem = [&items](QObject* object, const QString& matchName) {
            if(object->objectName() == matchName) {
                items.insert(object);
            }
        };

        addItem(parent, childName);

        std::function<void (QObject*, const QString&)> find
                = [&find, addItem](QObject* parent, const QString& name)
        {
            QSet<QObject*> children;

            if(dynamic_cast<QQuickItem*>(parent)) {
                const auto childItems = static_cast<QQuickItem*>(parent)->childItems();
                for(auto childItem : childItems) {
                    children.insert(childItem);
                }
            }

            for(auto child : parent->children()) {
                children.insert(child);
            }

            for(auto child : children) {
                addItem(child, name);
            }

            for(auto child : children) {
                find(child, name);
            }
        };

        find(parent, childName);

        return items;
    };

    const auto candidates = findChildren(parent, chain.last());

    for(auto candidate :  candidates) {
        //qDebug() << "Canditate:" << candidate << "To chain:" << toChain(candidate) << (chain == toChain(candidate));
        if(chain == toChain(candidate) && dynamic_cast<QQuickItem*>(candidate)) {
            return static_cast<QQuickItem*>(candidate);
        }
    }

    return nullptr;
}

QStringList ObjectFinder::toChain(const QObject *object)
{
    auto parent = dynamic_cast<const QQuickItem*>(object);

    QList<const QObject*> parents;
    while(parent) {
        parents.prepend(parent);
        parent = parent->parentItem();
    }

    QStringList names;
    for(auto obj : parents) {
        if(!obj->objectName().isEmpty()) {
            names.append(obj->objectName());
        }
    }

    return names;
}

QString ObjectFinder::toChainString(const QStringList &chain)
{
    return chain.join(seperator);
}

QString ObjectFinder::toChainString(const QObject *object)
{
    return toChainString(toChain(object));
}
