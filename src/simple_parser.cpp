#include "simple_parser.hpp"

#include <algorithm>
#include <numeric>
#include <vector>
#include <list>

// constants

// min package lenght
constexpr size_t min_pckg_lenght = 7;

// bytecodes
constexpr uint8_t startByte = 0x53;
constexpr uint8_t pollCode = 0x60;
constexpr uint8_t buzCode = 0x6A;

// general indexes
constexpr uint8_t startIndx = 0;
constexpr uint8_t addrIndx = 1;
constexpr uint8_t szLIndx = 2;
constexpr uint8_t szRIndx = 3;
constexpr uint8_t sqnIndx = 4;
constexpr uint8_t cmdIndx = 5;

// BUZ indexes
constexpr uint8_t readerIndx = 6;
constexpr uint8_t toneIndx = 7;
constexpr uint8_t onTmIndx = 8;
constexpr uint8_t offTmIndx = 9;
constexpr uint8_t countIndx = 10;

uint8_t getchecksum(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end)
{
    int whole_checksum = 0;
    uint8_t checksum;

    std::for_each(begin, end, [&whole_checksum, &checksum] (uint8_t n) {
        whole_checksum = whole_checksum + n;
        checksum = ~(0xFF & whole_checksum) + 1;
    });

    return checksum;
}

POLL::POLL(std::vector<uint8_t>& pack)
{
    m_addr = pack[addrIndx];
    m_sqn = pack[sqnIndx];
}

std::string POLL::to_string() const
{
    std::string str {"type:POLL,addr:" + std::to_string(m_addr)
        + ",sqn:" + std::to_string(m_sqn)};
    return str;
}

BUZ::BUZ(std::vector<uint8_t>& pack)
{
    m_addr = pack[addrIndx];
    m_sqn = pack[sqnIndx];
    m_reader = pack[readerIndx];
    m_tone = pack[toneIndx];
    m_on = pack[8];
    m_off = pack[9];
    m_count = pack[10];
}

std::string BUZ::to_string() const
{
    std::string str {"type:BUZ,addr:" + std::to_string(m_addr)
        + ",sqn:" + std::to_string(m_sqn)
        + ",reader:" + std::to_string(m_reader)
        + ",tone:" + std::to_string(m_tone)
        + ",on:" + std::to_string(m_on)
        + ",off:" + std::to_string(m_off)
        + ",count:" + std::to_string(m_count)};
    return str;
}

// add b-byte to buffer
void SimpleParser::push(uint8_t b) {m_buffer.push_front(b);}

// currently unused method for clearing buffer
void SimpleParser::reset() {m_buffer.clear();}

std::shared_ptr<Packet> SimpleParser::get_packet() const
{
    // parse one incoming package and clearing buffer
    std::vector<uint8_t> pack;

    // skip garbage input
    bool first_byte_recieved = false;
    while(!first_byte_recieved && m_buffer.size())
    {
        if(m_buffer.back() == startByte)
            first_byte_recieved = true;
        else
            m_buffer.pop_back();
    }

    // parse one package only
    if (m_buffer.size()) do
    {
        pack.push_back(std::move(m_buffer.back()));

        m_buffer.pop_back();

        if(!m_buffer.size()) break;

    } while (m_buffer.back() != startByte);

    // validate input
    if (validate_input(pack))
    {
        // get command code
        const uint8_t comcode = pack[cmdIndx];
        switch(comcode)
        {
            case pollCode:
                return std::shared_ptr<Packet>(new POLL(pack));
            case buzCode:
                return std::shared_ptr<Packet>(new BUZ(pack));
            default:
                // Unknown command code (just in case)
                return nullptr;
        }
    }
    else
    {
        // invalide package
        return nullptr;
    }
}

bool SimpleParser::validate_input(std::vector<uint8_t>& pack)
{
    // check min package lenght
    if(pack.size() < min_pckg_lenght) return false;

    // check startByte presence 
    if(pack.front() != startByte) return false;

    // is lenght of package valid?
    uint16_t pack_lenght = (uint16_t(pack[szRIndx]) << 8) + uint16_t(pack[szLIndx]);
    if(pack_lenght != pack.size()) return false;

    // check control sum
    uint8_t ctrlsum = std::accumulate(std::cbegin(pack), std::cend(pack), 0,
        [](uint8_t sum, uint8_t x) { return sum + x; });
    if(ctrlsum != getchecksum(pack.cbegin(), pack.cend())) return false;

    // package is valid
    return true;
}