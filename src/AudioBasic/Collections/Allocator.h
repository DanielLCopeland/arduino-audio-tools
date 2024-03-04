#pragma once
#include <stdlib.h>

#include "AudioConfig.h"
#include "AudioTools/AudioLogger.h"

namespace audio_tools {

/**
 * @brief Memory allocateator which uses malloc.
 * @ingroup collections
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class Allocator {
 public:
  template <class T>
  T* create() {
    T& ref = *(T*)allocate(sizeof(T));
    // ref();
    return &ref;
  }

  virtual void* allocate(size_t size) {
    void* result = do_allocate(size);
    if (result == nullptr) {
      LOGE("Allocateation failed for %d bytes", size);
      stop();
    } else {
      LOGI("Allocated %d", size);
    }
    return result;
  }

  virtual void free(void* memory) {
    if (memory != nullptr) ::free(memory);
  }

 protected:
  virtual void* do_allocate(size_t size) { return calloc(1, size); }
};

/**
 * @brief Memory allocateator which uses ps_malloc (on the ESP32) and if this
 * fails it resorts to malloc.
 * @ingroup collections
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class AllocatorExt : public Allocator {
  void* do_allocate(int size) {
    void* result = nullptr;
#ifdef ESP32
    result = ps_malloc(size);
#endif
    if (result == nullptr) result = malloc(size);
    if (result == nullptr) {
      LOGE("allocateation failed for %d bytes", size);
      stop();
    }
    // initialize object
    memset(result, 0, size);
    return result;
  }
};

#ifdef ESP32

*@brief Memory allocateator which uses ps_malloc to allocate the memory in PSRAM
    on the ESP32* @ingroup collections* @author Phil
        Schatzmann* @copyright GPLv3

    class AllocatorPSRAM : public Allocator {
  void* do_allocate(int size) {
    void* result = nullptr;
    result = ps_calloc(1, size);
    if (result == nullptr) {
      LOGE("allocateation failed for %d bytes", size);
      stop();
    }
    return result;
  }
};

#endif

static AllocatorExt DefaultAllocator;

}  // namespace audio_tools