#include "fuzz_common.hh"

#include <qpdf/QPDFAcroFormDocumentHelper.hh>
#include <qpdf/QPDFOutlineDocumentHelper.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFPageLabelDocumentHelper.hh>
#include <qpdf/QPDFPageObjectHelper.hh>

class PagesFuzzHelper: public fuzz::PDFFuzzHelper
{
  public:
    PagesFuzzHelper(unsigned char const* data, size_t size) :
        PDFFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        std::cerr << "\ninfo: starting test_pages\n";
        test_pages();
    }

  private:
    void
    test_pages()
    {
        // Parse all content streams, and exercise some helpers that operate on pages.
        QPDF qpdf;
        load_pdf(qpdf);
        info("load_pdf done");

        QPDFPageDocumentHelper pdh(qpdf);
        QPDFPageLabelDocumentHelper pldh(qpdf);
        QPDFOutlineDocumentHelper odh(qpdf);
        QPDFAcroFormDocumentHelper afdh(qpdf);

        afdh.generateAppearancesIfNeeded();
        info("generateAppearancesIfNeeded done");

        pdh.flattenAnnotations();
        info("flattenAnnotations done");

        int pageno = 0;
        for (auto& page : pdh.getAllPages()) {
            ++pageno;
            try {
                info("start page", pageno);

                page.coalesceContentStreams();
                info("coalesceContentStreams done");

                page.parseContents(nullptr);
                info("parseContents done");

                page.getImages();
                info("getImages done");

                pldh.getLabelForPage(pageno);
                info("getLabelForPage done");

                QPDFObjectHandle page_obj(page.getObjectHandle());
                page_obj.getJSON(JSON::LATEST, true).unparse();
                info("getJSON done");

                odh.getOutlinesForPage(page_obj);
                info("getOutlinesForPage done");

                for (auto& aoh : afdh.getWidgetAnnotationsForPage(page)) {
                    afdh.getFieldForAnnotation(aoh);
                }
            } catch (std::runtime_error const& e) {
                std::cerr << "page " << pageno << ": " << e.what() << '\n';
            }
        }
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    fuzz_init();
    PagesFuzzHelper f(data, size);
    f.run();
    return 0;
}
