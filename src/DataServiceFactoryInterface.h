//
// Created by arans on 6/06/2021.
//

#ifndef ZMQ_DATA_STREAM_CPP_DATASERVICEFACTORYINTERFACE_H
#define ZMQ_DATA_STREAM_CPP_DATASERVICEFACTORYINTERFACE_H

#include <memory>
#include "DataServiceReaderInterface.h"
#include "DataServiceWriterInterface.h"

class DataServiceFactoryInterface {
public:
    virtual std::shared_ptr<DataServiceReaderInterface>
    createReader(const std::string &serviceName, const std::string &serviceLocatorUrl) = 0;

    virtual std::shared_ptr<DataServiceWriterInterface>
    createWriter(const std::string &serviceName, const std::string &dataUrl, const std::string &serviceLocatorUrl) = 0;
};

#endif //ZMQ_DATA_STREAM_CPP_DATASERVICEFACTORYINTERFACE_H
