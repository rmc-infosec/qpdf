#include "fuzz_common.hh"

#include <qpdf/JSON.hh>

class JSONFuzzHelper: public fuzz::FilterFuzzHelper
{
  public:
    JSONFuzzHelper(unsigned char const* data, size_t size) :
        FilterFuzzHelper(data, size)
    {
    }

  protected:
    void
    test() override
    {
        // Test JSON parsing
        try {
            JSON::parse(std::string(reinterpret_cast<char const*>(data_), size_));
        } catch (std::runtime_error& e) {
            std::cerr << "runtime_error parsing json: " << e.what() << '\n';
        }

        // Test creating QPDF from JSON
        QPDF qpdf;
        qpdf.setMaxWarnings(1000);
        Buffer buf(const_cast<unsigned char*>(data_), size_);
        auto is = std::make_shared<BufferInputSource>("json", &buf);
        qpdf.createFromJSON(is);
    }
};

extern "C" int
LLVMFuzzerTestOneInput(unsigned char const* data, size_t size)
{
    JSONFuzzHelper f(data, size);
    f.run();
    return 0;
}
