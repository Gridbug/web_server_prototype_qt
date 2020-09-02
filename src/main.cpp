#include <QCoreApplication>

#include <httpserver.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qttp::HttpServer* httpSrv = qttp::HttpServer::getInstance();
    if (!httpSrv->initialize()) {
        qDebug() << QObject::tr("ERROR: Http server inialization failed.");
        return -1;
    }

    auto action = httpSrv->createAction([] (qttp::HttpData& data) {
                                            QJsonObject& json = data.getResponse().getJson();
                                            json["hello"] = "world";
                                        });

    action->registerRoute(qttp::HttpMethod::GET, "/");
    action->registerRoute(qttp::HttpMethod::GET, "/hello");

    httpSrv->startServer();

    return a.exec();
}
