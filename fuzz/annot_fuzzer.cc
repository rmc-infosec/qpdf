// Fuzzer for annotation handling operations
// Targets transformAnnotations, fixCopiedAnnotations, and annotation helpers

#include "fuzz_common.hh"

#include <qpdf/QPDFAcroFormDocumentHelper.hh>
#include <qpdf/QPDFAnnotationObjectHelper.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFPageObjectHelper.hh>

#include <set>
#include <vector>

class AnnotFuzzHelper: public fuzz::PDFFuzzHelper
{
  public:
    AnnotFuzzHelper(unsigned char const* data, size_t size) :
        PDFFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        std::cerr << "\ninfo: starting test_annotations\n";
        test_annotations();
    }

  private:
    void
    test_annotations()
    {
        QPDF qpdf;
        load_pdf(qpdf);
        QPDFPageDocumentHelper pdh(qpdf);
        QPDFAcroFormDocumentHelper afdh(qpdf);

        auto pages = pdh.getAllPages();
        if (pages.empty()) {
            return;
        }

        // Test annotation operations on each page
        int pageno = 0;
        for (auto& page : pages) {
            ++pageno;
            if (pageno > 5) {
                break; // Limit pages to avoid timeouts
            }

            try {
                info("processing page", pageno);

                // Get all annotations on the page
                info("getAnnotations (all)");
                auto all_annots = page.getAnnotations();

                // Get specific annotation types
                info("getAnnotations (Link)");
                page.getAnnotations("Link");

                info("getAnnotations (Widget)");
                auto widget_annots = page.getAnnotations("Widget");

                info("getAnnotations (Text)");
                page.getAnnotations("Text");

                // Test annotation object helper on each annotation
                for (auto& annot : all_annots) {
                    info("testing annotation helper");

                    // Get annotation properties
                    annot.getSubtype();
                    annot.getAppearanceState();
                    annot.getAppearanceDictionary();

                    // Get flags
                    annot.getFlags();
                }

                // Test widget annotations with form helper
                for (auto& widget : widget_annots) {
                    info("getFieldForAnnotation");
                    afdh.getFieldForAnnotation(widget);
                }

            } catch (std::runtime_error const& e) {
                std::cerr << "page " << pageno << ": " << e.what() << '\n';
            }
        }

        // Test transformAnnotations if we have at least 2 pages
        if (pages.size() >= 2) {
            try {
                auto& from_page = pages[0];
                auto& to_page = pages[1];

                // Get /Annots array from the source page
                auto from_page_obj = from_page.getObjectHandle();
                auto annots = from_page_obj.getKey("/Annots");

                if (annots.isArray()) {
                    info("transformAnnotations");

                    std::vector<QPDFObjectHandle> new_annots;
                    std::vector<QPDFObjectHandle> new_fields;
                    std::set<QPDFObjGen> old_fields;

                    // Get transformation matrix
                    auto matrix = from_page.getMatrixForTransformations();

                    // Transform annotations from one page to another
                    afdh.transformAnnotations(annots, new_annots, new_fields, old_fields, matrix);

                    info("transformAnnotations produced " + std::to_string(new_annots.size()) +
                         " annotations");
                }

                // Test fixCopiedAnnotations (works on pages directly)
                info("fixCopiedAnnotations");
                std::set<QPDFObjGen> fields_set;
                afdh.fixCopiedAnnotations(
                    to_page.getObjectHandle(), from_page_obj, afdh, &fields_set);

            } catch (std::runtime_error const& e) {
                std::cerr << "transform: " << e.what() << '\n';
            }
        }

        // Test AcroForm validation and operations
        try {
            info("afdh operations");

            // Validate form structure
            afdh.validate(true);

            // Get form fields
            auto fields = afdh.getFormFields();
            info("found " + std::to_string(fields.size()) + " form fields");

            // Test each form field
            int field_count = 0;
            for (auto& field : fields) {
                ++field_count;
                if (field_count > 10) {
                    break;
                }

                // Get field properties
                field.getFieldType();
                field.getFullyQualifiedName();
                field.getPartialName();
                field.getValue();
                field.getDefaultValue();
                field.getValueAsString();
                field.getDefaultValueAsString();
                field.isText();
                field.isCheckbox();
                field.isRadioButton();
                field.isChoice();

                // Get annotations for field
                afdh.getAnnotationsForField(field);
            }

            // Test need appearances
            afdh.getNeedAppearances();
            afdh.setNeedAppearances(false);

            // Disable digital signatures (exercises signature handling code)
            info("disableDigitalSignatures");
            afdh.disableDigitalSignatures();

        } catch (std::runtime_error const& e) {
            std::cerr << "afdh: " << e.what() << '\n';
        }

        // Write output
        try {
            info("writing output");
            write_pdf(qpdf, [](QPDFWriter& w) { w.setDeterministicID(true); });
        } catch (std::runtime_error const& e) {
            std::cerr << "write: " << e.what() << '\n';
        }
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    fuzz_init();
    AnnotFuzzHelper f(data, size);
    f.run();
    return 0;
}
