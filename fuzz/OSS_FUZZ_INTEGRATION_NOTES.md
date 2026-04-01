# OSS-Fuzz Integration Notes

Last updated: 2026-02-16

This document collects concrete evidence for OSS-Fuzz integration quality and review.

## 1) Evidence: two fixed memory-safety bugs

1. Heap-buffer-overflow in JSON handling
   - Issue: https://github.com/qpdf/qpdf/issues/1123
   - Public report includes AFL++/ASan reproduction details in the issue discussion.
   - Fix commit: https://github.com/qpdf/qpdf/commit/cb0f390cc1f98a8e82b27259f8f3cd5f162992eb
   - Commit message includes `fixes #1123`.

2. Heap-use-after-free in annotation transformation
   - Issue: https://github.com/qpdf/qpdf/issues/1620
   - Public report includes ASan traceback and PoC.
   - Fix PR: https://github.com/qpdf/qpdf/pull/1621
   - Merge commit: https://github.com/qpdf/qpdf/commit/75a3ef18bce817bd3fc06ad93117776f03ff390d
   - PR text includes `Fixes #1620`.

## 2) OSS-Fuzz packaging completeness

Change implemented in this branch:
- `fuzz/CMakeLists.txt` now installs each existing fuzzer's optional
  `*.options` and `*.dict` files automatically, instead of hardcoding only
  `qpdf_fuzzer.options`.

Why this matters:
- Per-target options and dictionaries are now consistently packaged for OSS-Fuzz.
- New targets (for example `annot_fuzzer`) no longer require manual install-list updates.

## 4) Submission checklist

Prepare these links/artifacts before submitting:
- [ ] Merged PR URL(s) for new/refactored fuzz target(s)
- [ ] CIFuzz run URL(s): `.github/workflows/cifuzz.yml`
- [ ] Main CI URL(s): `.github/workflows/main.yml` (`build-fuzzer` job)
- [ ] OSS-Fuzz coverage delta screenshot/link (before/after)
- [ ] Two fixed bug links (issue + fixing commit/PR), listed above
- [ ] Short note explaining which code paths the new target exercises

Useful local verification commands:

```bash
cmake --build build --target fuzzers -j"$(nproc)"
ctest --test-dir build -R '^fuzz$' --output-on-failure
```
