# CI verification

`python ci/run.py` builds and runs the existing tests on Linux, Windows and
macOS. Linux needs GCC, make, libcurl and UUID development headers; macOS needs
Xcode command-line tools; Windows needs Visual Studio 2022 and its Windows SDK.
The Windows job uses the bundled Cygwin utilities and discovers compiler versions.

The harness supplies the legacy tests' fixtures and directory layout, and a
local HTTP proxy answers the curl test's fixed URL. It runs each test separately
and uses `sh -e` so recursive make failures cannot be hidden. Build/test logs and
a JSON summary are written to `test-results/` and uploaded by GitHub Actions.

The optional `makeXLAddin` Office COM automation helper needs installed Excel
and is omitted. Native Excel bindings still build on Windows.

Use `--skip-build` to reuse existing libraries, `--jobs 2` to control concurrency,
and `--make-arg NAME=value` for site-specific build settings.
