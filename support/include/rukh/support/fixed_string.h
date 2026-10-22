#ifndef RUKH_SUPPORT_FIXED_STRING_H_INCLUDED
#define RUKH_SUPPORT_FIXED_STRING_H_INCLUDED

#include <cstddef>
#include <cstring>
#include <exception>
#include <functional>
#include <string>
#include <string_view>

#include <rukh/support/hash.h>

namespace ShashChess
{

    template <std::size_t Capacity> class FixedString
    {
        public:
            FixedString() : length_(0)
            {
                data_[0] = '\0';
            }

            FixedString(const char* value)
            {
                const std::size_t length = std::strlen(value);
                if (length > Capacity)
                    std::terminate();
                std::memcpy(data_, value, length);
                length_ = length;
                data_[length_] = '\0';
            }

            FixedString(const std::string& value)
            {
                if (value.size() > Capacity)
                    std::terminate();
                std::memcpy(data_, value.data(), value.size());
                length_ = value.size();
                data_[length_] = '\0';
            }

            std::size_t
            size() const
            {
                return length_;
            }

            std::size_t
            capacity() const
            {
                return Capacity;
            }

            const char*
            c_str() const
            {
                return data_;
            }

            const char*
            data() const
            {
                return data_;
            }

            char&
            operator[](std::size_t index)
            {
                return data_[index];
            }

            const char&
            operator[](std::size_t index) const
            {
                return data_[index];
            }

            FixedString&
            operator+=(const char* value)
            {
                const std::size_t length = std::strlen(value);
                if (length_ + length > Capacity)
                    std::terminate();
                std::memcpy(data_ + length_, value, length);
                length_ += length;
                data_[length_] = '\0';
                return *this;
            }

            FixedString&
            operator+=(const FixedString& other)
            {
                return (*this += other.c_str());
            }

            operator std::string() const
            {
                return std::string(data_, length_);
            }

            operator std::string_view() const
            {
                return std::string_view(data_, length_);
            }

            template <typename T>
            bool
            operator==(const T& other) const noexcept
            {
                return static_cast<std::string_view>(*this) == other;
            }

            template <typename T>
            bool
            operator!=(const T& other) const noexcept
            {
                return static_cast<std::string_view>(*this) != other;
            }

            void
            clear()
            {
                length_ = 0;
                data_[0] = '\0';
            }

        private:
            char        data_[Capacity + 1];
            std::size_t length_;
    };

} // namespace ShashChess

template <std::size_t N> struct std::hash<ShashChess::FixedString<N>>
{
        std::size_t
        operator()(const ShashChess::FixedString<N>& value) const noexcept
        {
            return ShashChess::hash_bytes(value.data(), value.size());
        }
};

#endif // RUKH_SUPPORT_FIXED_STRING_H_INCLUDED
