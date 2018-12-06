#include "text_itemizer.h"

#include <memory>

#include "log.h"
#include "unicode_utils.h"

BidiResult resolveBidi(const std::vector<uint32_t>& code_points,
                       Range range,
                       bool paraDir) {
  std::vector<std::pair<Range, bool>> runs;
  if (range.empty()) {
    return BidiResult({paraDir, runs});
  }
  // Fuck ICU API. They only accept UTF-16
  std::vector<uint16_t> utf16;
  for (int32_t i = range.start(); i < range.end(); ++i) {
    utf16.push_back(static_cast<uint16_t>(code_points[i]));
  }

  UErrorCode status = U_ZERO_ERROR;
  UBiDiUniquePtr ubidi(ubidi_open());
  UBiDiLevel request = paraDir ? UBIDI_DEFAULT_LTR : UBIDI_DEFAULT_RTL;
  ubidi_setPara(ubidi.get(), reinterpret_cast<const UChar*>(utf16.data()),
                utf16.size(), request, nullptr, &status);
  CHECK(U_SUCCESS(status));

  bool paraIsRtl = ubidi_getParaLevel(ubidi.get()) & 0x01;
  uint32_t runCount = ubidi_countRuns(ubidi.get(), &status);
  CHECK(U_SUCCESS(status));
  CHECK(runCount >= 0);

  if (runCount == 0) {
    return BidiResult({paraIsRtl, runs});
  } else if (runCount == 1) {
    bool runDir =
        ubidi_getVisualRun(ubidi.get(), 0, nullptr, nullptr) == UBIDI_RTL;
    runs.push_back(std::make_pair(range, runDir));
    return BidiResult({paraIsRtl, runs});
  } else {
    for (uint32_t i = 0; i < runCount; ++i) {
      int32_t start = -1;
      int32_t length = -1;
      bool runDir =
          ubidi_getVisualRun(ubidi.get(), i, &start, &length) == UBIDI_RTL;
      CHECK(start != -1);
      CHECK(length != -1);
      start += range.start();
      runs.push_back(std::make_pair(Range(start, start + length), runDir));
    }
    return BidiResult({paraIsRtl, runs});
  }
}
