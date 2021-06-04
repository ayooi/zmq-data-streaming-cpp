//
// Created by arans on 4/06/2021.
//

#include <gtest/gtest.h>
#include "WriterConnectionDetail.h"

namespace writerconnectiondetailtest {
    TEST(WriterConnectionDetail, tcp_parse_test) {
        auto res = parse("tcp://address.hostname:15245");

        EXPECT_EQ(TCP, res.type);
        EXPECT_EQ("address.hostname", res.address);
        EXPECT_EQ(15245, res.listenPort);
    }

    TEST(WriterConnectionDetail, inproc_parse_test) {
        auto res = parse("inproc://banana");
        EXPECT_EQ(INPROC, res.type);
        EXPECT_EQ("banana", res.address);
    }

    TEST(WriterConnectionDetail, ipc_parse_test) {
        auto res = parse("ipc://derp");

        EXPECT_EQ(IPC, res.type);
        EXPECT_EQ("derp", res.address);
    }


}