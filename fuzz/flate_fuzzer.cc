#include "fuzz_common.hh"

class FlateFuzzHelper: public fuzz::FilterFuzzHelper
{
  public:
    FlateFuzzHelper(unsigned char const* data, size_t size) :
        FilterFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        Pl_Discard discard;
        Pl_Flate p("decode", &discard, Pl_Flate::a_deflate);
        p.write(const_cast<unsigned char*>(data_), size_);
        p.finish();
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    FlateFuzzHelper f(data, size);
    f.run();
    return 0;
}
