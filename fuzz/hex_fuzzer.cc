#include "fuzz_common.hh"

#include <qpdf/Pl_ASCIIHexDecoder.hh>

class HexFuzzHelper: public fuzz::FilterFuzzHelper
{
  public:
    HexFuzzHelper(unsigned char const* data, size_t size) :
        FilterFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        Pl_Discard discard;
        Pl_ASCIIHexDecoder p("decode", &discard);
        p.write(const_cast<unsigned char*>(data_), size_);
        p.finish();
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    HexFuzzHelper f(data, size);
    f.run();
    return 0;
}
