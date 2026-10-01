#include <boost/container/vector.hpp>
#include "Core/RevetteCore.h"



namespace rvl {

template <typename T>
using efficient_vector = boost::container::vector<
    T,
    void,
    boost::container::vector_options<boost::container::stored_size<u32>>::type
>;

}
