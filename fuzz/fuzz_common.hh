// Common infrastructure for QPDF fuzzers
// Provides base classes and utilities to reduce boilerplate

#ifndef FUZZ_COMMON_HH
#define FUZZ_COMMON_HH

#include <qpdf/Buffer.hh>
#include <qpdf/BufferInputSource.hh>
#include <qpdf/Pl_DCT.hh>
#include <qpdf/Pl_Discard.hh>
#include <qpdf/Pl_Flate.hh>
#include <qpdf/Pl_PNGFilter.hh>
#include <qpdf/Pl_RunLength.hh>
#include <qpdf/Pl_TIFFPredictor.hh>
#include <qpdf/QPDF.hh>
#include <qpdf/QPDFExc.hh>
#include <qpdf/QPDFWriter.hh>

#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace fuzz
{
    // Set all memory limits used during fuzzing. Call this once at the start of each fuzz run.
    // This configures limits for:
    // - DCT (JPEG) decompression memory and scan limits
    // - PNG predictor memory limit
    // - RunLength decoder memory limit
    // - TIFF predictor memory limit
    // - Flate compression/decompression memory limit
    // Also sets DCT to throw on corrupt data rather than attempting to decompress.
    inline void
    set_pipeline_limits()
    {
        // Limit the memory used to decompress JPEG files during fuzzing. Excessive memory use
        // during fuzzing is due to corrupt JPEG data which sometimes cannot be detected before
        // jpeg_start_decompress is called. During normal use of qpdf very large JPEGs can
        // occasionally occur legitimately and therefore must be allowed during normal operations.
        Pl_DCT::setMemoryLimit(100'000'000);
        Pl_DCT::setScanLimit(50);

        // Do not decompress corrupt data. This may cause extended runtime within jpeglib without
        // exercising additional code paths in qpdf, and potentially causing counterproductive
        // timeouts.
        Pl_DCT::setThrowOnCorruptData(true);

        Pl_PNGFilter::setMemoryLimit(1'000'000);
        Pl_RunLength::setMemoryLimit(1'000'000);
        Pl_TIFFPredictor::setMemoryLimit(1'000'000);
        Pl_Flate::memory_limit(200'000);
    }

    // Base class for all fuzzers. Provides common exception handling and timing infrastructure.
    class FuzzHelperBase
    {
      public:
        FuzzHelperBase() :
            start_(std::chrono::steady_clock::now())
        {
        }

        virtual ~FuzzHelperBase() = default;

        // Main entry point. Sets pipeline limits and catches all expected exceptions.
        void
        run()
        {
            set_pipeline_limits();
            try {
                test();
            } catch (std::runtime_error const& e) {
                std::cerr << "runtime_error: " << e.what() << '\n';
            }
        }

      protected:
        // Override this to implement the fuzzer's test logic.
        virtual void test() = 0;

        // Log a timing message for timeout analysis.
        void
        info(std::string const& msg, int pageno = 0) const
        {
            const std::chrono::duration<double> elapsed{std::chrono::steady_clock::now() - start_};
            std::cerr << elapsed.count() << " info - " << msg;
            if (pageno > 0) {
                std::cerr << " page " << pageno;
            }
            std::cerr << '\n';
        }

      private:
        const std::chrono::time_point<std::chrono::steady_clock> start_;
    };

    // Base class for fuzzers that process PDF input using a QPDF object.
    // Handles input buffer management and pipeline limits.
    class PDFFuzzHelper: public FuzzHelperBase
    {
      public:
        PDFFuzzHelper(unsigned char const* data, size_t size) :
            // We do not modify data, so it is safe to remove the const for Buffer
            input_buffer_(const_cast<unsigned char*>(data), size)
        {
        }


      protected:
        // Load PDF from the input buffer into an existing QPDF object.
        void
        load_pdf(QPDF& qpdf)
        {
            auto is = std::make_shared<BufferInputSource>("fuzz input", &input_buffer_);
            qpdf.setMaxWarnings(200);
            qpdf.processInputSource(is);
        }

        // Write a QPDF object, discarding output. Used to exercise QPDFWriter.
        // Sets decode level to all to ensure full stream decoding coverage.
        void
        write_pdf(QPDF& qpdf, std::function<void(QPDFWriter&)> configure = nullptr)
        {
            QPDFWriter w(qpdf);
            w.setOutputPipeline(&discard_);
            w.setDecodeLevel(qpdf_dl_all);
            if (configure) {
                configure(w);
            }
            w.write();
        }

        Pl_Discard discard_;

      private:
        Buffer input_buffer_;
    };

    // Base class for simple filter fuzzers that process raw data through a pipeline.
    class FilterFuzzHelper: public FuzzHelperBase
    {
      public:
        FilterFuzzHelper(unsigned char const* data, size_t size) :
            data_(data),
            size_(size)
        {
        }

      protected:
        unsigned char const* data_;
        size_t size_;
    };

} // namespace fuzz

// Common setup for LLVMFuzzerTestOneInput that handles JSIMD workaround
inline void
fuzz_init()
{
#ifndef _WIN32
    // Used by jpeg library to work around false positives in memory sanitizer.
    setenv("JSIMD_FORCENONE", "1", 1);
#endif
}

#endif // FUZZ_COMMON_HH
