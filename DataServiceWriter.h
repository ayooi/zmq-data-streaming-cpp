//
// Created by arans on 3/06/2021.
//

#ifndef ZMQ_DATA_STREAM_CPP_DATASERVICEWRITER_H
#define ZMQ_DATA_STREAM_CPP_DATASERVICEWRITER_H

class DataServiceWriter {
public:

    DataServiceWriter(const std::string &serviceName,
                      const std::string &dataUrl,
                      void *ctx,
                      const std::string &serviceLocatorUrl);

    ~DataServiceWriter();

    void write(const uint8_t *payload, const size_t length);


private:
    void *_dataSocket;
    void *_serviceSocket;

    std::thread _thread;
    bool _running;
    const std::string _dataUrl;
    const std::string _serviceName;
};

#endif //ZMQ_DATA_STREAM_CPP_DATASERVICEWRITER_H
