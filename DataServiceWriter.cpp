//
// Created by arans on 3/06/2021.
//

#include <vector>
#include <set>
#include <list>
#include <thread>
#include <zmq.h>
#include <iostream>
#include "DataServiceWriter.h"

using namespace std::chrono_literals;

DataServiceWriter::DataServiceWriter(const std::string &serviceName,
                                     const std::string &dataUrl,
                                     void *ctx,
                                     const std::string &serviceLocatorUrl)
        : _running(true),
          _serviceName(serviceName),
          _dataUrl(dataUrl) {
    _dataSocket = zmq_socket(ctx, ZMQ_PUSH);
    long val = 1000;
    zmq_setsockopt(_dataSocket, ZMQ_SNDHWM, &val, sizeof(long));
    zmq_bind(_dataSocket, dataUrl.c_str());

    _serviceSocket = zmq_socket(ctx, ZMQ_DEALER);
    zmq_setsockopt(_serviceSocket, ZMQ_IDENTITY, &dataUrl, dataUrl.length());
    zmq_connect(_serviceSocket, serviceLocatorUrl.c_str());

    _thread = std::thread([&]() {
        while (_running) {
            std::string command = "register";
            zmq_send(_serviceSocket, command.c_str(), command.length(), ZMQ_SNDMORE);
            zmq_send(_serviceSocket, _serviceName.c_str(), _serviceName.length(), ZMQ_SNDMORE);
            zmq_send(_serviceSocket, _dataUrl.c_str(), _dataUrl.length(), 0);
            std::this_thread::sleep_for(5s);
        }
    });
}

DataServiceWriter::~DataServiceWriter() {
    _running = false;
    {
        std::string command = "deregister";
        zmq_send(_serviceSocket, command.c_str(), command.length(), ZMQ_SNDMORE);
        zmq_send(_serviceSocket, _serviceName.c_str(), _serviceName.length(), ZMQ_SNDMORE);
        zmq_send(_serviceSocket, _dataUrl.c_str(), _dataUrl.length(), 0);
    }
    zmq_close(_serviceSocket);
    zmq_close(_dataSocket);
    _thread.join();
}

void DataServiceWriter::write(const uint8_t *payload, const size_t length) {
    zmq_send(_dataSocket, payload, length, 0);
}
