/// @file
/// Defines the StringSlice object.
#ifndef _SCOTTZ0R_STRING_SLICE_INCLUDE_GUARD
#define _SCOTTZ0R_STRING_SLICE_INCLUDE_GUARD

#include <cstddef>
#include <limits>

namespace scottz0r
{
    /// @brief Non owning slice of a string. This has the same lifetime as the m_str pointer.
    class StringSlice
    {
    public:
        using size_type = std::size_t;
        static constexpr size_type npos = std::numeric_limits<size_type>::max();

        StringSlice() noexcept
            : m_str(nullptr), m_size(0)
        {

        }

        /// @brief Construct with a string and size. Null characters will be included.
        StringSlice(const char* str, size_type size) noexcept
            : m_str(str), m_size(size)
        {

        }

        /// @brief Construct with a string. This will loop until a null character is found. Does not include terminating
        ///        null character.
        StringSlice(const char* str) noexcept
            : m_str(str), m_size(0)
        {
            const char* p_str = m_str;
            while (*p_str != 0)
            {
                ++p_str;
            }

            m_size = p_str - m_str;
        }

        bool empty() const noexcept { return m_size == 0; }

        const char* data() const noexcept { return m_str; }

        size_type size() const noexcept { return m_size; }

        char at(size_type i) const noexcept
        {
            if (i < m_size)
            {
                return m_str[i];
            }

            return 0;
        }

        char operator[](size_type i) const noexcept
        {
            return m_str[i];
        }

        size_type find(char c, size_type start = 0) const noexcept
        {
            if (start >= m_size)
            {
                return npos;
            }

            for (size_type i = start; i < m_size; ++i)
            {
                if (m_str[i] == c)
                {
                    return i;
                }
            }

            return npos;
        }

        StringSlice substr(size_type pos, size_type len = npos) const noexcept
        {
            if (pos < m_size)
            {
                size_type real_len = m_size - pos;
                real_len = real_len < len ? real_len : len;
                return StringSlice(m_str + pos, real_len);
            }

            return StringSlice();
        }

        StringSlice rstrip() noexcept
        {
            if (m_size == 0)
            {
                return StringSlice();
            }

            size_type i = m_size - 1;
            for (; i > 0; --i)
            {
                if (!is_whitespace(m_str[i]))
                {
                    break;
                }
            }

            // Return empty slice if reached the first character and it should be trimmed.
            if (i == 0 && is_whitespace(m_str[i]))
            {
                return StringSlice();
            }

            return StringSlice(m_str, i + 1);
        }

        StringSlice lstrip() noexcept
        {
            size_type i = 0;
            for (; i < m_size; ++i)
            {
                if (!is_whitespace(m_str[i]))
                {
                    break;
                }
            }

            return StringSlice(m_str + i, m_size - i);
        }

        StringSlice strip() noexcept
        {
            return rstrip().lstrip();
        }

        bool operator==(const StringSlice& other) const noexcept
        {
            if (other.m_size != m_size)
            {
                return false;
            }

            for (size_type i = 0; i < m_size; ++i)
            {
                if (m_str[i] != other.m_str[i])
                {
                    return false;
                }
            }

            return true;
        }

        bool operator!=(const StringSlice& other) const noexcept
        {
            return !(*this == other);
        }

    private:

        bool is_whitespace(char c) noexcept
        {
            return c == '\r' || c == '\n' || c == '\t' || c == ' ';
        }

        const char* m_str;
        size_type m_size;
    };

    template<size_t Size>
    constexpr StringSlice to_slice(const char(&str)[Size]) noexcept
    {
        return StringSlice(str, Size - 1);
    }

    inline StringSlice get_line(const StringSlice& slice) noexcept
    {
        for (StringSlice::size_type i = 0; i < slice.size(); ++i)
        {
            if (slice[i] == '\n')
            {
                // Add one to size to pick up newline character.
                return StringSlice(slice.data(), i + 1);
            }
        }

        // Return entire slice if a newline was not found.
        return slice;
    }

    inline int to_int(const StringSlice& slice, std::size_t* pos = nullptr, int radix = 10) noexcept
    {
        char min = '0';
        char max = '0' + (radix - 1);

        int result = 0;
        int mutilplier = 1;

        std::size_t i = 0;

        if (!slice.empty())
        {
            if (slice[0] == '-')
            {
                mutilplier = -1;
                ++i;
            }
            else if (slice[0] == '+')
            {
                ++i;
            }
        }

        for (; i < slice.size(); ++i)
        {
            if (slice[i] >= min && slice[i] <= max)
            {
                result *= radix;
                result += slice[i] - min;
            }
            else
            {
                break;
            }
        }

        if (pos)
        {
            *pos = i;
        }

        return mutilplier * result;
    }
}

#endif // _SCOTTZ0R_STRING_SLICE_INCLUDE_GUARD
