#ifndef RUKH_SUPPORT_VALUE_LIST_H_INCLUDED
#define RUKH_SUPPORT_VALUE_LIST_H_INCLUDED

#include <cassert>
#include <cstddef>

namespace ShashChess
{

    template <typename T, std::size_t MaxSize> class ValueList
    {
        public:
            std::size_t
            size() const
            {
                return size_;
            }

            int
            ssize() const
            {
                return int(size_);
            }

            void
            push_back(const T& value)
            {
                assert(size_ < MaxSize);
                values_[size_++] = value;
            }

            const T*
            begin() const
            {
                return values_;
            }

            const T*
            end() const
            {
                return values_ + size_;
            }

            const T&
            operator[](int index) const
            {
                return values_[index];
            }

            T*
            make_space(std::size_t count)
            {
                T* result = &values_[size_];
                size_ += count;
                assert(size_ <= MaxSize);
                return result;
            }

        private:
            T           values_[MaxSize];
            std::size_t size_ = 0;
    };

} // namespace ShashChess

#endif // RUKH_SUPPORT_VALUE_LIST_H_INCLUDED
