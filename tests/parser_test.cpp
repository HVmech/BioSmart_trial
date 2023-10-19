#include <gtest/gtest.h>

#include <vector>
#include "simple_parser.hpp"

TEST(ParserTest, BasicPacket) {
    SimpleParser sp;
    std::vector<uint8_t> input_data = { 0x53, 0x01, 0x07, 0x00, 0x02, 0x60, 0x43 };

    for (auto b : input_data)
    {
        sp.push(b);
    }
    
    auto packet = sp.get_packet();
    ASSERT_NE(packet, nullptr);
    ASSERT_STREQ(packet->to_string().c_str(),"type:POLL,addr:1,sqn:2");
}

TEST(ParserTest, WrongCRC) {
    SimpleParser sp;
    std::vector<uint8_t> input_data = { 0x53, 0x01, 0x07, 0x00, 0x02, 0x60, 0x55 };

    for (auto b : input_data)
    {
        sp.push(b);
    }
    
    auto packet = sp.get_packet();
    ASSERT_EQ(packet, nullptr);
}

TEST(ParserTest, WaitForStart) {
    SimpleParser sp;
    std::vector<uint8_t> input_data = { 0x01, 0x02, 0x03, 0x53, 0x01, 0x07, 0x00, 0x02, 0x60, 0x43 };

    for (auto b : input_data)
    {
        sp.push(b);
    }
    
    auto packet = sp.get_packet();
    ASSERT_NE(packet, nullptr);
    ASSERT_STREQ(packet->to_string().c_str(),"type:POLL,addr:1,sqn:2");
}

TEST(ParserTest, BzzPacket) {
    SimpleParser sp;
    std::vector<uint8_t> input_data = { 0x53, 0x01, 0x0C, 0x00, 0x00, 0x6A, 0x00, 0x02, 0x06, 0x06, 0x03, 0x25 };

    for (auto b : input_data)
    {
        sp.push(b);
    }

    auto packet = sp.get_packet();
    ASSERT_NE(packet, nullptr);
    ASSERT_STREQ(packet->to_string().c_str(),"type:BUZ,addr:1,sqn:0,reader:0,tone:2,on:6,off:6,count:3");
}

// My tests

// empty package
TEST(ParserTest, EmptyPacket) {
    SimpleParser sp;

    auto packet = sp.get_packet();
    ASSERT_EQ(packet, nullptr);
}

// garbage package
TEST(ParserTest, GarbagePacket) {
    SimpleParser sp;
    std::vector<uint8_t> input_data = { 0x01, 0x02, 0x03, 0x01, 0x07, 0x00, 0x02, 0x60, 0x43 };

    for (auto b : input_data)
    {
        sp.push(b);
    }
    
    auto packet = sp.get_packet();
    ASSERT_EQ(packet, nullptr);
}

// unknown package
TEST(ParserTest, UnknownPacket) {
    SimpleParser sp;
    std::vector<uint8_t> input_data = { 0x53, 0x01, 0x07, 0x00, 0x02, 0x88, 0x43 };

    for (auto b : input_data)
    {
        sp.push(b);
    }
    
    auto packet = sp.get_packet();
    ASSERT_EQ(packet, nullptr);
}