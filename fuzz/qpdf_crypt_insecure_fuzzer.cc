#include "fuzz_common.hh"

#include <qpdf/QPDFPageObjectHelper.hh>

class CryptInsecureFuzzHelper: public fuzz::PDFFuzzHelper
{
  public:
    CryptInsecureFuzzHelper(unsigned char const* data, size_t size) :
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
        // Write with R3 insecure encryption to exercise QPDFWriter
        QPDF qpdf;
        load_pdf(qpdf);

        write_pdf(qpdf, [](QPDFWriter& w) {
            w.setStaticID(true);
            w.setObjectStreamMode(qpdf_o_disable);
            w.setR3EncryptionParametersInsecure(
                "u", "o", true, true, true, true, true, true, qpdf_r3p_full);
        });
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    fuzz_init();
    CryptInsecureFuzzHelper f(data, size);
    f.run();
    return 0;
}
