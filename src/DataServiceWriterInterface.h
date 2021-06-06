//
// Created by arans on 6/06/2021.
//

#ifndef ZMQ_DATA_STREAM_CPP_DATASERVICEWRITERINTERFACE_H
#define ZMQ_DATA_STREAM_CPP_DATASERVICEWRITERINTERFACE_H

class DataServiceWriterInterface {
public:
    virtual void write(const uint8_t *payload, size_t length) = 0;
};

#endif //ZMQ_DATA_STREAM_CPP_DATASERVICEWRITERINTERFACE_H
