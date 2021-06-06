//
// Created by arans on 6/06/2021.
//

#include "DataServiceFactory.h"
#include "DataServiceReader.h"
#include "DataServiceWriter.h"

DataServiceFactory::DataServiceFactory(void *ctx)
        : _ctx(ctx) {

}

DataServiceFactory::~DataServiceFactory() {

}

std::shared_ptr<DataServiceReaderInterface>
DataServiceFactory::createReader(const std::string &serviceName, const std::string &serviceLocatorUrl) {
    return std::make_shared<DataServiceReader>("unique-name", serviceName,
                                               _ctx,
                                               serviceLocatorUrl);
}

std::shared_ptr<DataServiceWriterInterface>
DataServiceFactory::createWriter(const std::string &serviceName,
                                 const std::string &dataUrl,
                                 const std::string &serviceLocatorUrl) {
    return std::make_shared<DataServiceWriter>(serviceName,
                                               parse(dataUrl),
                                               _ctx,
                                               serviceLocatorUrl);
}

