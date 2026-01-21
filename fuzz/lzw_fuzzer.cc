#include "fuzz_common.hh"

#include <qpdf/Pl_LZWDecoder.hh>

class LZWFuzzHelper: public fuzz::FilterFuzzHelper
{
  public:
    LZWFuzzHelper(unsigned char const* data, size_t size) :
        FilterFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        Pl_Discard discard;
        Pl_LZWDecoder p("decode", &discard, false);
        p.write(const_cast<unsigned char*>(data_), size_);
        p.finish();
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    LZWFuzzHelper f(data, size);
    f.run();
    return 0;
}
