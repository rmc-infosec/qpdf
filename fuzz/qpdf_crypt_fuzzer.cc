#include "fuzz_common.hh"

#include <qpdf/QPDFPageObjectHelper.hh>

class CryptFuzzHelper: public fuzz::PDFFuzzHelper
{
  public:
    CryptFuzzHelper(unsigned char const* data, size_t size) :
        PDFFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        std::cerr << "\ninfo: starting test_write\n";
        test_write();
    }

  private:
    void
    test_write()
    {
        // Write with R6 encryption to exercise QPDFWriter
        QPDF qpdf;
        load_pdf(qpdf);

        write_pdf(qpdf, [](QPDFWriter& w) {
            w.setStaticID(true);
            w.setLinearization(true);
            w.setR6EncryptionParameters(
                "u", "o", true, true, true, true, true, true, qpdf_r3p_full, true);
        });
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    fuzz_init();
    CryptFuzzHelper f(data, size);
    f.run();
    return 0;
}
