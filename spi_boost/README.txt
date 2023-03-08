This is a sub-set of the most excellent boost library.

We only use boost for shared pointers in SPI, and some of these types are
publicly exported from the SPI shared library.

Thus there is a risk that if the client library is also using boost, then
there might be some incompatibility between the templates used by SPI using
one version of boost (in our case 1.51) and the templates used by the
client library using a different version of boost.

To avoid this problem we take a small sub-set of boost and change the
namespace from boost to spi_boost, including all macros, header file
guards etc. We also put the files into this separate directory spi_boost
to make it clear that this code really originates from boost instead of
from spi.

