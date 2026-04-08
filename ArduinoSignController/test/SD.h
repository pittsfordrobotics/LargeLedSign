#ifndef TEST_SD_H
#define TEST_SD_H

#include <cstddef>
#include <cstdint>
#include <vector>

class File
{
    public:
        File() = default;
        explicit File(const std::vector<uint8_t>& data) : m_data(data) {}

        bool seek(uint32_t position)
        {
            if (position > m_data.size())
            {
                return false;
            }

            m_position = position;
            return true;
        }

        size_t read(uint8_t* buffer, size_t length)
        {
            if (buffer == nullptr || length == 0)
            {
                return 0;
            }

            const size_t available = m_data.size() - m_position;
            const size_t toRead = (length < available) ? length : available;

            for (size_t i = 0; i < toRead; ++i)
            {
                buffer[i] = m_data[m_position + i];
            }

            m_position += toRead;
            return toRead;
        }

        int read()
        {
            if (m_position >= m_data.size())
            {
                return -1;
            }

            return m_data[m_position++];
        }

        size_t size() const
        {
            return m_data.size();
        }

        explicit operator bool() const
        {
            return !m_data.empty();
        }

    private:
        std::vector<uint8_t> m_data{};
        size_t m_position{0};
};

#endif // TEST_SD_H
