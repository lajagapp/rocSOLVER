/* ************************************************************************
 * Copyright (c) 2020 Advanced Micro Devices, Inc.
 *
 * ************************************************************************ */

#include "testing_latrd.hpp"

using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::ValuesIn;
using namespace std;

typedef std::tuple<vector<int>, vector<int>> latrd_tuple;

// each matrix_size_range is a {n, lda, ldw}

// each op_range is a {k, ul}
// if ul = 0, then uplo = 'L'
// if ul = 1, then uplo = 'U'

// case when n = 0 and k = 0 will also execute the bad arguments test
// (null handle, null pointers and invalid values)

// for checkin_lapack tests
const vector<vector<int>> matrix_size_range = {
    // quick return
    {0, 1, 1},
    // invalid
    {-1, 1, 1},
    {20, 5, 20},
    {20, 20, 5},
    // normal (valid) samples
    {35, 50, 50},
    {70, 100, 70},
    {130, 130, 150},
    {150, 150, 150}};

const vector<vector<int>> op_range = {
    // quick return
    {0, 0},
    // invalid
    {-1, 0},
    {180, 0},
    // normal (valid) samples
    {10, 0},
    {25, 1},
    {30, 0},
    {30, 1}};

// for daily_lapack tests
const vector<vector<int>> large_matrix_size_range
    = {{152, 152, 152}, {640, 640, 656}, {1000, 1024, 1000}};

const vector<vector<int>> large_op_range = {{64, 0}, {98, 1}, {130, 0}, {150, 1}};

Arguments latrd_setup_arguments(latrd_tuple tup)
{
    vector<int> matrix_size = std::get<0>(tup);
    vector<int> op_size = std::get<1>(tup);

    Arguments arg;

    arg.N = matrix_size[0];
    arg.lda = matrix_size[1];
    arg.ldb = matrix_size[2]; //ldw
    arg.K = op_size[0];
    arg.uplo_option = op_size[1] ? 'U' : 'L';

    arg.timing = 0;

    return arg;
}

class LATRD : public ::TestWithParam<latrd_tuple>
{
protected:
    LATRD() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_P(LATRD, __float)
{
    Arguments arg = latrd_setup_arguments(GetParam());

    if(arg.K == 0 && arg.N == 0)
        testing_latrd_bad_arg<float>();

    arg.batch_count = 1;
    testing_latrd<float>(arg);
}

TEST_P(LATRD, __double)
{
    Arguments arg = latrd_setup_arguments(GetParam());

    if(arg.K == 0 && arg.N == 0)
        testing_latrd_bad_arg<double>();

    arg.batch_count = 1;
    testing_latrd<double>(arg);
}

TEST_P(LATRD, __float_complex)
{
    Arguments arg = latrd_setup_arguments(GetParam());

    if(arg.K == 0 && arg.N == 0)
        testing_latrd_bad_arg<rocblas_float_complex>();

    arg.batch_count = 1;
    testing_latrd<rocblas_float_complex>(arg);
}

TEST_P(LATRD, __double_complex)
{
    Arguments arg = latrd_setup_arguments(GetParam());

    if(arg.K == 0 && arg.N == 0)
        testing_latrd_bad_arg<rocblas_double_complex>();

    arg.batch_count = 1;
    testing_latrd<rocblas_double_complex>(arg);
}

INSTANTIATE_TEST_SUITE_P(daily_lapack,
                         LATRD,
                         Combine(ValuesIn(large_matrix_size_range), ValuesIn(large_op_range)));

INSTANTIATE_TEST_SUITE_P(checkin_lapack,
                         LATRD,
                         Combine(ValuesIn(matrix_size_range), ValuesIn(op_range)));
