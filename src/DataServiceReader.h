//
// Created by arans on 3/06/2021.
//

#ifndef ZMQ_DATA_STREAM_CPP_DATASERVICEREADER_H
#define ZMQ_DATA_STREAM_CPP_DATASERVICEREADER_H

#include <string>
#include <set>
#include <thread>
#include <zmq.h>
#include <iostream>
#include "DataServiceReaderInterface.h"

class DataServiceReader : public DataServiceReaderInterface {
public:
    DataServiceReader(const std::string &uniqueName,
                      std::string serviceName,
                      void *ctx,
                      const std::string &serviceLocatorUrl);

    void doConnects(const std::set<std::string> &incoming);

    void takeAll(std::list<std::vector<uint8_t>> &res) override;

    ~DataServiceReader();

private:
    void *_locatorSocket;
    void *_dataSocket;
    void *_controlSocket;

    std::string _serviceName;
    std::thread _queryThread;
    std::thread _processingThread;
    bool _running;

    std::list<std::vector<uint8_t>> _payloads;
    std::set<std::string> _locations;

};

#endif //ZMQ_DATA_STREAM_CPP_DATASERVICEREADER_H
