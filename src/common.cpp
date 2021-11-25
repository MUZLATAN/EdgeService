#include "common.h"

namespace meta {
namespace vision {

GlobalVariable* GetGlobalVariable(void) {
    static GlobalVariable t;
    return &t;
};

}  // namespace vision
}  // namespace meta