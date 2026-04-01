#include "fuzz_common.hh"

class RunLengthFuzzHelper: public fuzz::FilterFuzzHelper
{
  public:
    RunLengthFuzzHelper(unsigned char const* data, size_t size) :
        FilterFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        Pl_RunLength::setMemoryLimit(1'000'000);
        Pl_Discard discard;
        Pl_RunLength p("decode", &discard, Pl_RunLength::a_decode);
        p.write(const_cast<unsigned char*>(data_), size_);
        p.finish();
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    RunLengthFuzzHelper f(data, size);
    f.run();
    return 0;
}
