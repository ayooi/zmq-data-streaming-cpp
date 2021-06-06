//
// Created by arans on 6/06/2021.
//

#ifndef ZMQ_DATA_STREAM_CPP_DATASERVICEREADERINTERFACE_H
#define ZMQ_DATA_STREAM_CPP_DATASERVICEREADERINTERFACE_H

#include <list>
#include <vector>

class DataServiceReaderInterface {
public:
    virtual void takeAll(std::list<std::vector<uint8_t>> &res) = 0;
};

#endif //ZMQ_DATA_STREAM_CPP_DATASERVICEREADERINTERFACE_H
