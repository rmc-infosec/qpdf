#include "fuzz_common.hh"

class DCTFuzzHelper: public fuzz::FilterFuzzHelper
{
  public:
    DCTFuzzHelper(unsigned char const* data, size_t size) :
        FilterFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        // Limit the memory used to decompress JPEG files during fuzzing. Excessive memory use
        // during fuzzing is due to corrupt JPEG data which sometimes cannot be detected before
        // jpeg_start_decompress is called. During normal use of qpdf very large JPEGs can
        // occasionally occur legitimately and therefore must be allowed during normal operations.
        Pl_DCT::setMemoryLimit(200'000'000);
        Pl_DCT::setScanLimit(50);

        // Do not decompress corrupt data. This may cause extended runtime within jpeglib without
        // exercising additional code paths in qpdf.
        Pl_DCT::setThrowOnCorruptData(true);

        Pl_Discard discard;
        Pl_DCT p("decode", &discard);
        p.write(const_cast<unsigned char*>(data_), size_);
        p.finish();
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    fuzz_init();
    DCTFuzzHelper f(data, size);
    f.run();
    return 0;
}
