This dir is used by `src/*/CMakeLists.txt` to run `git describe --match "v*"`.
Because, if it would be run in that directory, on Windows the `--match "v*"`
would get expanded to the `version.h.cmake` file there, causing Git to
obviously not find any such tag. The reason is probably that on Windows the
wildcard expansion is done on the application side and not by the shell, thus
being performed even though CMake docs say `execute_process()` doesn't involve
a shell. This directory is thus dedicated for that operation, *guaranteed* to
never contain any file starting with `v` (or `V` for that matter because,
again, HELLO WINDOWS).

No matter what you do, DON'T put any files starting with `v` or `V` here, or
hell breaks loose again.
