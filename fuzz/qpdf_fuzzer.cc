#include "fuzz_common.hh"

#include <qpdf/QPDFPageObjectHelper.hh>

class QPDFFuzzHelper: public fuzz::PDFFuzzHelper
{
  public:
    QPDFFuzzHelper(unsigned char const* data, size_t size) :
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
        // Write in various ways to exercise QPDFWriter
        QPDF qpdf;
        load_pdf(qpdf);

        write_pdf(qpdf, [](QPDFWriter& w) {
            w.setDeterministicID(true);
            w.setQDFMode(true);
        });
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    fuzz_init();
    QPDFFuzzHelper f(data, size);
    f.run();
    return 0;
}
