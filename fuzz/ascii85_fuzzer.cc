#include "fuzz_common.hh"

#include <qpdf/Pl_ASCII85Decoder.hh>

class ASCII85FuzzHelper: public fuzz::FilterFuzzHelper
{
  public:
    ASCII85FuzzHelper(unsigned char const* data, size_t size) :
        FilterFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        Pl_Discard discard;
        Pl_ASCII85Decoder p("decode", &discard);
        p.write(const_cast<unsigned char*>(data_), size_);
        p.finish();
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    ASCII85FuzzHelper f(data, size);
    f.run();
    return 0;
}
