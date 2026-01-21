#include "fuzz_common.hh"

#include <qpdf/QPDFOutlineDocumentHelper.hh>
#include <qpdf/QPDFPageObjectHelper.hh>

#include <list>

class OutlinesFuzzHelper: public fuzz::PDFFuzzHelper
{
  public:
    OutlinesFuzzHelper(unsigned char const* data, size_t size) :
        PDFFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        std::cerr << "\ninfo: starting test_outlines\n";
        test_outlines();
    }

  private:
    void
    test_outlines()
    {
        QPDF qpdf;
        load_pdf(qpdf);

        std::list<std::vector<QPDFOutlineObjectHelper>> queue;
        auto& odh = QPDFOutlineDocumentHelper::get(qpdf);
        queue.push_back(odh.getTopLevelOutlines());

        while (!queue.empty()) {
            for (auto& ol : *(queue.begin())) {
                ol.getDestPage();
                queue.push_back(ol.getKids());
            }
            queue.pop_front();
        }
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    fuzz_init();
    OutlinesFuzzHelper f(data, size);
    f.run();
    return 0;
}
