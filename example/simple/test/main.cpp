#include <QtQuickTest/quicktest.h>

//Qt includes
#include <QtQuickTest>
#include <QQmlEngine>
#include <QQmlContext>
#include <QGuiApplication>

class Setup : public QObject
{
    Q_OBJECT

public:
    Setup() {}

public slots:
    void qmlEngineAvailable(QQmlEngine *engine)
    {
        //Init custom handlers here, this allows use to find the QmlTextRecorder
        //from the example folder
        engine->addImportPath("../../");
    }
};

//This is the replacement for, so we can set EnableHighDpiScaling
//QUICK_TEST_MAIN_WITH_SETUP(mapWhere-qml-tests, Setup)
int main(int argc, char **argv) \
{
    QTEST_SET_MAIN_SOURCE_PATH
    Setup setup;
    const char* qmlSourceDirectory = QUICK_TEST_SOURCE_DIR; //The source directory where the qml tests are located
    return quick_test_main_with_setup(argc, argv, "mapWhere-qml-tests", qmlSourceDirectory, &setup); \
}



#include "main.moc"
