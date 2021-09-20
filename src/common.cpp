#include "common.h"

namespace algo {
namespace vision {

GlobalVariable* GetGlobalVariable(void) {
    static GlobalVariable t;
    return &t;
};

}  // namespace vision
}  // namespace algo