#include <iostream>
#include <sstream>
#include <functional>
#include <tuple>
#include <vector>

/**
 * Standard Murmur3 algorithm thats accepts key value in byte array and its length
 * return hashing result
 */
uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed = 0) {
    uint32_t h = seed;
    if (len > 3) {
        const uint32_t* key_x4 = (const uint32_t*) key;
        size_t i = len >> 2;
        do {
            uint32_t k = *key_x4++;
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;
            h ^= k;
            h = (h << 13) | (h >> 19);
            h = (h * 5) + 0xe6546b64;
        } while (--i);
        key = (const uint8_t*) key_x4;
    }
    if (len & 3) {
        size_t i = len & 3;
        uint32_t k = 0;
        key = &key[i - 1];
        do {
            k <<= 8;
            k |= *key--;
        } while (--i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k;
    }
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

// std::index_sequence implementation

template<size_t ...>
struct sequence { };

template<size_t N, size_t... Is>
struct generate : generate<N - 1, N - 1, Is...> { };

template<size_t... Is>
struct generate<0, Is...> : sequence<Is...> { };

// End std::index_sequence implementation


/**
 * Accept variadic arguments and returns lambda function using std::function
 */
template <typename... Args>
std::function<std::tuple<Args...>()> getFunctor(Args&&... args)
{
    /**
     * Creates lambda expression that captures variadic argument args...
     * from the enclosing scope by reference and returns an std::tuple
     */
    return [&]()
    {
        // Forwarding of arguments taken as rvalue references to deduced type
        return std::tuple<Args...>(std::forward<Args>(args)...);
    };
}

/**
 * Function template convert_to_byte for (int, char, float, double)
 */
template<typename T>
void convert_to_byte(std::vector<unsigned char>& byteArr, const T & val)
{
    unsigned char const * inputPtr = reinterpret_cast<unsigned char const *>(&val);
    for (std::size_t k = 0; k < sizeof(T); k++)
    {
         byteArr.push_back(inputPtr[sizeof(T) - k - 1]);
    }
}

/**
 * Specialized version of convert_to_byte function which takes "char *"
 */
template<>
void convert_to_byte<char *>(std::vector<unsigned char>& byteArr, char * const & val)
{
    for (char* inputPtr = val; *inputPtr != '\0'; inputPtr++)
    {
        byteArr.push_back(*inputPtr);
    }
}

/**
 * Specialized version of convert_to_byte function which takes "std::string"
 */
template<>
void convert_to_byte<std::string>(std::vector<unsigned char>& byteArr, std::string const & val)
{
    for (std::size_t i = 0; i != val.size(); ++i)
    {
       byteArr.push_back(val[i]);
    }
}

template <typename... Args>
class murmur3
{
    private:
    
        /**
         * unpack the parameter to extract the value from the tuple, copy them to the string stream
         */
        template<typename T, size_t... Is>
        std::vector<unsigned char> for_each(T&& t, sequence<Is...>)
        {
            std::vector<unsigned char> byteArr;
            int dummy[] = {0, (( convert_to_byte(byteArr, std::get<Is>(t)) ), void(), 0)...};
            static_cast<void>(dummy); // avoid warning for unused variable
            return byteArr;
        }
    
    public:
    
        /**
         * Takes lambda function using std::function that returns a tuple of the key values
         * Unpacks the tuple that is created by lambda function and creates byte array and calls murmur3_32 hashing
         */
        uint32_t apply(std::function<std::tuple<Args...>()>&& func)
        {
            auto key_tuple = func();
            // unpacks the tuple
            std::vector<unsigned char> byteArr = for_each(key_tuple, generate<sizeof...(Args)>());
            return murmur3_32((uint8_t*) static_cast<unsigned char*>(byteArr.data()), byteArr.size());
        }
};

int main()
{
    
    char args_01[] = "Hello";
    std::string str = "Hello World";
    murmur3<char, int, float, double, char*, std::string> murmur3_obj;
    auto func = getFunctor('a', 12, 12.5f, 12.5, args_01, str);
    
    // returns hash result of the multikeys
    auto result = murmur3_obj.apply(std::move(func));
    
    std::cout << "Hash Result: " << result << std::endl;
    
    return 0;
}

