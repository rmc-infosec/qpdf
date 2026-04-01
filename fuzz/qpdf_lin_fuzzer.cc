#include "fuzz_common.hh"

#include <qpdf/QPDFPageObjectHelper.hh>

class LinFuzzHelper: public fuzz::PDFFuzzHelper
{
  public:
    LinFuzzHelper(unsigned char const* data, size_t size) :
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
        // Write with linearization to exercise QPDFWriter
        QPDF qpdf;
        load_pdf(qpdf);

        write_pdf(qpdf, [](QPDFWriter& w) {
            w.setDeterministicID(true);
            w.setObjectStreamMode(qpdf_o_generate);
            w.setLinearization(true);
        });
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    fuzz_init();
    LinFuzzHelper f(data, size);
    f.run();
    return 0;
}
