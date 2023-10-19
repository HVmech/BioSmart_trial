#ifndef SIMPLE_PARSER_HPP
#define SIMPLE_PARSER_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <list>

class Packet
{
public:
    virtual std::string to_string() const = 0;
    virtual ~Packet() = default;
};

class POLL : public Packet
{
public:
    POLL() = delete;
    POLL(std::vector<uint8_t>& pack);
    virtual std::string to_string() const;
    ~POLL() = default;
private:
    uint8_t m_addr, m_sqn;
};

class BUZ : public Packet
{
public:
    BUZ() = delete;
    BUZ(std::vector<uint8_t>& pack);
    virtual std::string to_string() const;
    ~BUZ() = default;
private:
    uint8_t m_addr, m_sqn, m_reader, m_tone, m_on, m_off, m_count;
};

class SimpleParser
{
public:
    void push(uint8_t b);
    void reset();
    std::shared_ptr<Packet> get_packet() const;
private:
    static bool validate_input(std::vector<uint8_t>& pack);
    mutable std::list<uint8_t> m_buffer;
};

#endif // SIMPLE_PARSER_HPP