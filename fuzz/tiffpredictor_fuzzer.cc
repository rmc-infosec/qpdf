#include "fuzz_common.hh"

class TIFFPredictorFuzzHelper: public fuzz::FilterFuzzHelper
{
  public:
    TIFFPredictorFuzzHelper(unsigned char const* data, size_t size) :
        FilterFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        Pl_Discard discard;
        Pl_TIFFPredictor p("decoder", &discard, Pl_TIFFPredictor::a_decode, 16, 1, 8);
        p.write(const_cast<unsigned char*>(data_), size_);
        p.finish();

        // Exercise with strange values for some of the parameters.
        Pl_TIFFPredictor p2("decoder", &discard, Pl_TIFFPredictor::a_decode, 16, 2, 5);
        p2.write(const_cast<unsigned char*>(data_), size_);
        p2.finish();
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    TIFFPredictorFuzzHelper f(data, size);
    f.run();
    return 0;
}
