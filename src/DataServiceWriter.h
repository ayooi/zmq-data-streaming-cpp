//
// Created by arans on 3/06/2021.
//

#ifndef ZMQ_DATA_STREAM_CPP_DATASERVICEWRITER_H
#define ZMQ_DATA_STREAM_CPP_DATASERVICEWRITER_H

#include <vector>
#include <set>
#include <list>
#include <zmq.h>
#include <string>
#include <thread>
#include "WriterConnectionDetail.h"

class DataServiceWriter {
public:

    DataServiceWriter(std::string serviceName,
                      const WriteConnectionDetail &connectionDetail,
                      void *ctx,
                      const std::string &serviceLocatorUrl);

    ~DataServiceWriter();

    void write(const uint8_t *payload, size_t length);


private:
    void *_dataSocket;
    void *_serviceSocket;

    std::thread _thread;
    bool _running;
    std::string _dataBindUrl;
    std::string _dataAnnouncementUrl;
    std::string _serviceName;
};

#endif //ZMQ_DATA_STREAM_CPP_DATASERVICEWRITER_H
