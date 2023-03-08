If you link with this library then you will be overriding new/delete throughout.
So only do it if you really want to do that.

The upside is that you can then call spi_util::AllocsNotFreed() and possibly spi_util::MemoryNotFreed()
to get interesting information.

