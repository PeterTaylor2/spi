# SPI in the browser

This optional, single-threaded Emscripten target builds the SPI static runtime,
bundled utilities and zlib. It also provides a small Embind interface for creating,
reading and serializing objects from JavaScript. Native builds are independent.

## Build and test

Install and activate [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
6.0.13, then run from the repository root:

```sh
source /path/to/emsdk/emsdk_env.sh
make -C wasm -j2
npm ci --prefix wasm
cd wasm
npx playwright install chromium
make test
```

On Linux, use `npx playwright install --with-deps chromium` to also install browser
system dependencies. CI builds and runs the tests in headless Chromium.

The outputs are `wasm/build/libspi.a`, `spi.mjs` and `spi.wasm`. Serve the two
module files together over HTTP(S), with `.mjs` served as JavaScript and `.wasm`
as `application/wasm`.

## JavaScript

```js
import createSPI from './spi.mjs';

const spi = await createSPI();
const original = new spi.Object();
let copy;
try {
    original.setString('name', 'Example');
    original.setInt('count', 42);
    copy = spi.Object.fromString(original.toString('TEXT'));
    console.log(copy.getString('name'), copy.getInt('count'));
} finally {
    copy?.delete();
    original.delete();
}
```

`Object` also has `typeName`, `getDouble`/`setDouble`, `getBool`/`setBool`, and
`getObject`/`setObject`. Each returned object owns a reference and must be
released with `.delete()`. Setters replace that handle's object; previously
stored child references retain their previous value.

`fromString` accepts SPI text and JSON. `toString` takes an explicit format:
`'TEXT'` or `'JSON'`. SPI's JSON reader represents numbers as doubles, so use
`getDouble` for numeric fields read from JSON. This interface exposes scalar
and nested-object fields; arrays, dates, binary serialization and filesystem
operations remain available through the C++ library.

C++ errors propagate through Emscripten's exception mechanism. When catching an
SPI exception in JavaScript, read and release it using the exported helpers:

```js
try {
    const object = spi.Object.fromString(input);
    try { /* use object */ } finally { object.delete(); }
} catch (exception) {
    // JavaScript errors (e.g. invalid Embind arguments) are ordinary Errors.
    if (exception instanceof Error) throw exception;
    let message;
    try { message = spi.getExceptionMessage(exception)[1]; }
    finally { spi.decrementExceptionRefcount(exception); }
    throw new Error(message);
}
```

## Linking an application

Compile application sources and generated C++ wrappers with `em++`,
`-std=c++17 -fexceptions -DSPI_STATIC -DSPI_UTIL_STATIC` and the repository root
on the include path. Link them with `build/libspi.a` and `-fexceptions`; add
`--bind` if providing Embind bindings. Use `bindings.cpp` as a small example.
Generate any application wrappers using the native SPI code generators before
cross-compiling. This target uses the runtime's already checked-in generated
sources, so building the supplied module does not require native generators.

The binary format keeps its existing 32-bit encoded dimensions on both native
64-bit builds and wasm32. Tests cover text, JSON, binary and compressed text,
shared-object references, generated classes, malformed input, dimension overflow,
browser UUIDs and Emscripten's in-memory filesystem.

## Scope

This target excludes curl/URL loading, UDP upload and recording, dynamic native
plugins, and Python/Excel/.NET bindings. Applications can fetch data in JavaScript
and pass text to `fromString`. Filesystem operations use Emscripten's virtual
filesystem, not unrestricted access to the user's disk. There are no browser
worker/thread bindings or automatic JavaScript code generation in this initial
integration.
