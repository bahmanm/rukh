#ifndef RUKH_SUPPORT_PRNG_H_INCLUDED
#define RUKH_SUPPORT_PRNG_H_INCLUDED

#include <cassert>
#include <cstdint>

namespace ShashChess
{

    class PRNG
    {
            std::uint64_t state_;

            std::uint64_t
            rand64()
            {
                state_ ^= state_ >> 12;
                state_ ^= state_ << 25;
                state_ ^= state_ >> 27;
                return state_ * 2685821657736338717ULL;
            }

        public:
            explicit PRNG(std::uint64_t seed) : state_(seed)
            {
                assert(seed);
            }

            template <typename T>
            T
            rand()
            {
                return T(rand64());
            }

            template <typename T>
            T
            sparse_rand()
            {
                return T(rand64() & rand64() & rand64());
            }
    };

} // namespace ShashChess

#endif // RUKH_SUPPORT_PRNG_H_INCLUDED
