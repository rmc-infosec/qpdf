#include "fuzz_common.hh"

class PNGPredictorFuzzHelper: public fuzz::FilterFuzzHelper
{
  public:
    PNGPredictorFuzzHelper(unsigned char const* data, size_t size) :
        FilterFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        Pl_Discard discard;
        Pl_PNGFilter p("decode", &discard, Pl_PNGFilter::a_decode, 32, 1, 8);
        p.write(const_cast<unsigned char*>(data_), size_);
        p.finish();
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    PNGPredictorFuzzHelper f(data, size);
    f.run();
    return 0;
}
