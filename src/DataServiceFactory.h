//
// Created by arans on 6/06/2021.
//

#ifndef ZMQ_DATA_STREAM_CPP_DATASERVICEFACTORY_H
#define ZMQ_DATA_STREAM_CPP_DATASERVICEFACTORY_H

#include "DataServiceFactoryInterface.h"
#include <zmq.h>

class DataServiceFactory : public DataServiceFactoryInterface {
public:
    DataServiceFactory(void *ctx);

    ~DataServiceFactory();

    std::shared_ptr<DataServiceReaderInterface>
    createReader(const std::string &serviceName,
                 const std::string &serviceLocatorUrl) override;

    std::shared_ptr<DataServiceWriterInterface>
    createWriter(const std::string &serviceName,
                 const std::string &dataUrl,
                 const std::string &serviceLocatorUrl) override;

private:
    void *_ctx;
};


#endif //ZMQ_DATA_STREAM_CPP_DATASERVICEFACTORY_H
