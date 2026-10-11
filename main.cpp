#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <random>
#include <string>

#include <immintrin.h>

using namespace std::string_literals;


namespace 
{

    uint64_t l1_norm_column(const std::vector<uint64_t>& matrix, size_t n)
    {
        uint64_t sum = 0;
        for (size_t j = 0; j < n; ++j) 
        {
            for (size_t i = 0; i < n; ++i) 
            {
                sum += matrix[i * n + j];  
            }
        }
        return sum;
    }

    uint64_t l1_norm_row(const std::vector<uint64_t>& matrix, size_t n)
    {
        uint64_t sum = 0;
        for (size_t i = 0; i < n; ++i) 
        {      
            for (size_t j = 0; j < n; ++j) 
            {
                sum += matrix[i * n + j];
            }
        }
        return sum;
    }


    std::vector<uint64_t> generate_test_matrix(size_t n, size_t lo, size_t hi)
    {
        std::vector<uint64_t> data(n * n);

        std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<uint64_t> dist(lo, hi);

        for (auto& x : data) {
            x = dist(rng);
        }

        return data;
    }

    uint64_t l1_norm_row_avx2(const std::vector<uint64_t>& matrix, size_t n)
    {
        
        const size_t total = n * n;
        const uint64_t* p = matrix.data();

        __m256i a0 = _mm256_setzero_si256();
        __m256i a1 = _mm256_setzero_si256();
        __m256i a2 = _mm256_setzero_si256();
        __m256i a3 = _mm256_setzero_si256();

        size_t i = 0;
        for (; i + 16 <= total; i += 16) {
            a0 = _mm256_add_epi64(a0, _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p + i +  0)));
            a1 = _mm256_add_epi64(a1, _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p + i +  4)));
            a2 = _mm256_add_epi64(a2, _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p + i +  8)));
            a3 = _mm256_add_epi64(a3, _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p + i + 12)));
        }

        alignas(32) uint64_t tmp[4];
        uint64_t sum = 0;

        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), a0);
        sum += tmp[0] + tmp[1] + tmp[2] + tmp[3];

        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), a1);
        sum += tmp[0] + tmp[1] + tmp[2] + tmp[3];

        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), a2);
        sum += tmp[0] + tmp[1] + tmp[2] + tmp[3];

        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), a3);
        sum += tmp[0] + tmp[1] + tmp[2] + tmp[3];

        for (; i < total; ++i) 
            sum += p[i];

        return sum;
    }

}



int main(int argc, char** argv)
{
    if(argc == 6 && argv[1] == "gen_matrix"s)
    {
        size_t n = std::stoull(argv[2], nullptr, 10);
        auto matrix = generate_test_matrix(n, std::stoull(argv[3], nullptr, 10), std::stoull(argv[4], nullptr, 10));
        
        std::ofstream out(argv[5], std::ios::binary);
        out.write(reinterpret_cast<char*>(&n), sizeof(n));
        out.write(reinterpret_cast<char*>(matrix.data()), matrix.size() * sizeof(uint64_t));
        return 1;
    }
    else if(argc == 4 && argv[1] == "test"s)
    {
        std::ifstream in(argv[2], std::ios::binary);
        
        size_t n;
        in.read(reinterpret_cast<char*>(&n), sizeof(n));
        std::vector<uint64_t> matrix(n * n, 0);
        in.read(reinterpret_cast<char*>(matrix.data()), n * n * sizeof(uint64_t));

        uint64_t l1_norm = 0;

        if(argv[3] == "row"s)
            l1_norm = l1_norm_row(matrix, n);
        else if(argv[3] == "column"s)
            l1_norm = l1_norm_column(matrix, n);
        else if(argv[3] == "avx"s)
        {
            l1_norm = l1_norm_row_avx2(matrix, n);
        }
        else
        {
            std::cout << "unknown mode test" << std::endl;
            return -1;
        }

        std::cout << "L1-norm = " << l1_norm << std::endl;
        return 1;

    }
    else
    {
        std::cout << "unknown mode" << std::endl;
        return -1;
    }

}
