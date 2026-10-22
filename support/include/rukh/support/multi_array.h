#ifndef RUKH_SUPPORT_MULTI_ARRAY_H_INCLUDED
#define RUKH_SUPPORT_MULTI_ARRAY_H_INCLUDED

#include <array>
#include <cstddef>
#include <type_traits>

namespace ShashChess
{

    template <typename T, std::size_t Size, std::size_t... Sizes> class MultiArray;

    namespace detail
    {

        template <typename T, std::size_t Size, std::size_t... Sizes> struct MultiArrayHelper
        {
                using ChildType = MultiArray<T, Sizes...>;
        };

        template <typename T, std::size_t Size> struct MultiArrayHelper<T, Size>
        {
                using ChildType = T;
        };

        template <typename To, typename From>
        constexpr bool is_strictly_assignable_v =
            std::is_assignable_v<To&, From> &&
            (std::is_same_v<To, From> || !std::is_convertible_v<From, To>);

    } // namespace detail

    template <typename T, std::size_t Size, std::size_t... Sizes> class MultiArray
    {
            using ChildType = typename detail::MultiArrayHelper<T, Size, Sizes...>::ChildType;
            using ArrayType = std::array<ChildType, Size>;
            ArrayType data_;

        public:
            using value_type = typename ArrayType::value_type;
            using size_type = typename ArrayType::size_type;
            using difference_type = typename ArrayType::difference_type;
            using reference = typename ArrayType::reference;
            using const_reference = typename ArrayType::const_reference;
            using pointer = typename ArrayType::pointer;
            using const_pointer = typename ArrayType::const_pointer;
            using iterator = typename ArrayType::iterator;
            using const_iterator = typename ArrayType::const_iterator;
            using reverse_iterator = typename ArrayType::reverse_iterator;
            using const_reverse_iterator = typename ArrayType::const_reverse_iterator;

            constexpr auto&
            at(size_type index)
            {
                return data_.at(index);
            }

            constexpr const auto&
            at(size_type index) const
            {
                return data_.at(index);
            }

            constexpr auto&
            operator[](size_type index) noexcept
            {
                return data_[index];
            }

            constexpr const auto&
            operator[](size_type index) const noexcept
            {
                return data_[index];
            }

            constexpr auto&
            front() noexcept
            {
                return data_.front();
            }

            constexpr const auto&
            front() const noexcept
            {
                return data_.front();
            }

            constexpr auto&
            back() noexcept
            {
                return data_.back();
            }

            constexpr const auto&
            back() const noexcept
            {
                return data_.back();
            }

            auto*
            data()
            {
                return data_.data();
            }

            const auto*
            data() const
            {
                return data_.data();
            }

            constexpr auto
            begin() noexcept
            {
                return data_.begin();
            }

            constexpr auto
            end() noexcept
            {
                return data_.end();
            }

            constexpr auto
            begin() const noexcept
            {
                return data_.begin();
            }

            constexpr auto
            end() const noexcept
            {
                return data_.end();
            }

            constexpr auto
            cbegin() const noexcept
            {
                return data_.cbegin();
            }

            constexpr auto
            cend() const noexcept
            {
                return data_.cend();
            }

            constexpr auto
            rbegin() noexcept
            {
                return data_.rbegin();
            }

            constexpr auto
            rend() noexcept
            {
                return data_.rend();
            }

            constexpr auto
            rbegin() const noexcept
            {
                return data_.rbegin();
            }

            constexpr auto
            rend() const noexcept
            {
                return data_.rend();
            }

            constexpr auto
            crbegin() const noexcept
            {
                return data_.crbegin();
            }

            constexpr auto
            crend() const noexcept
            {
                return data_.crend();
            }

            constexpr bool
            empty() const noexcept
            {
                return data_.empty();
            }

            constexpr size_type
            size() const noexcept
            {
                return data_.size();
            }

            constexpr size_type
            max_size() const noexcept
            {
                return data_.max_size();
            }

            template <typename U>
            void
            fill(const U& value)
            {
                static_assert(
                    detail::is_strictly_assignable_v<T, U>,
                    "Cannot assign fill value to entry type");
                for (auto& element : data_)
                {
                    if constexpr (sizeof...(Sizes) == 0)
                        element = value;
                    else
                        element.fill(value);
                }
            }

            constexpr void
            swap(
                MultiArray<
                    T,
                    Size,
                    Sizes...>& other) noexcept
            {
                data_.swap(other.data_);
            }
    };

} // namespace ShashChess

#endif // RUKH_SUPPORT_MULTI_ARRAY_H_INCLUDED
