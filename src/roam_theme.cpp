
#include "roam_theme.h"
#include "roam_tools.h"
using std::vector;

using namespace Tools;
namespace fs = std::filesystem;
string Term::fg, Term::bg;
string Fx::reset = reset_base;

namespace Theme {

    fs::path theme_dir;
    fs::path user_theme_dir;
    vector<string> themes;    
}