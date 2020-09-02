#include <QCoreApplication>

#include <httpserver.h>


class OperationTypesWebAction : public qttp::Action {
    // Action interface
public:
    void onGet(qttp::HttpData& data) override;
    const char* getName() const override;
    std::set<qttp::HttpPath> getRoutes() const override;
    const char* getSummary() const override;
    const char* getDescription() const override;
    QStringList getTags() const override;
    std::vector<qttp::Input> getInputs() const override;

private:
    const std::map<int, QString> typeNamesById = {
        {0, "BashCommandOperation"},
        {1, "PythonScriptOperation"},
        {2, "GnuPlotScriptOperation"},
    };
};

const char* OperationTypesWebAction::getSummary() const {
    return "Operation type getter";
}

const char* OperationTypesWebAction::getDescription() const {
    return "Returns operation type specification by its id.";
}

QStringList OperationTypesWebAction::getTags() const {
    return {"operation type", "operation", "get"};
}

std::set<qttp::HttpPath> OperationTypesWebAction::getRoutes() const {
    return {
        {qttp::HttpMethod::GET, "operation_type/:operation_type_id"}
    };
}

std::vector<qttp::Input> OperationTypesWebAction::getInputs() const {
    return {
        qttp::Input("operation_type_id", { {qttp::HttpMethod::GET, "operation_type/:operation_type_id"} })
    };
}

void OperationTypesWebAction::onGet(qttp::HttpData& data) {
    auto& httpResponse = data.getResponse();

    const QJsonObject& request = data.getRequest().getJson();
    QJsonValue typeIdValue = request["operation_type_id"];
    if (!typeIdValue.isString()) {
        httpResponse.setStatus(qttp::HttpStatus::BAD_REQUEST);
        httpResponse.getJson()["status"] = "Bad id";    //For some reason, it's necessary to return some value
        return;
    }

    bool status;
    int opTypeId = typeIdValue.toString().toInt(&status);
    if (!status || typeNamesById.count(opTypeId) == 0) {
        httpResponse.setStatus(qttp::HttpStatus::BAD_REQUEST);
        httpResponse.getJson()["status"] = "Bad id";    //For some reason, it's necessary to return some value
        return;
    }

    httpResponse.getJson()["operation_type_name"] = typeNamesById.at(opTypeId);
}

const char* OperationTypesWebAction::getName() const {
    return "operation_types_action";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qttp::HttpServer* httpSrv = qttp::HttpServer::getInstance();
    if (!httpSrv->initialize()) {
        qDebug() << QObject::tr("ERROR: Http server inialization failed.");
        return -1;
    }

    qttp::HttpServer::ServerInfo info = httpSrv->getServerInfo();
    info.title = "Submodules storage system";
    info.description = "An API that provides an access to the information stored in the Active Knowledge Database over the network.";
    info.companyName = "Supercomputer Software Department of Institute of Computational Mathematics and Mathematical Geophysics SB RAS";
    httpSrv->setServerInfo(info);

    httpSrv->addActionAndRegister<OperationTypesWebAction>();

    httpSrv->startServer();

    httpSrv->initHttpDirectory(QDir().absoluteFilePath("./www"));
    httpSrv->initSwagger(true);

    return a.exec();
}
