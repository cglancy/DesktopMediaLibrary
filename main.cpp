#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "maincontroller.h"
#include "treemodel.h"
#include "listmodel.h"
#include "progressimageprovider.h"
#include "thumbnailimageprovider.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainController *controller = new MainController(&app);
    controller->loadData();

    QQmlApplicationEngine engine;
    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("treeModel", controller->treeModel());
    rootContext->setContextProperty("listModel", controller->listModel());
    rootContext->setContextProperty("controller", controller);

    engine.addImageProvider("progress", controller->progressImageProvider());
    engine.addImageProvider("thumbnail", controller->thumbnailImageProvider());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
