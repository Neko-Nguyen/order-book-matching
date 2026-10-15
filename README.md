# Order Book Matching Engine

A limit order book and matching engine in modern C++ (C++20), built to be correct first and then measurably fast.

## Plan

1. **Primer.** Learn the domain: order books, order types, price-time priority. Done.
2. **Correct core engine.** Limit, market and cancel orders with price-time priority, unit tests, and a randomized test against a simple reference book.
3. **Benchmark, then optimize.** Replay a realistic order stream, report throughput and p50/p99/p99.9 latency, and record before/after numbers for each optimization.
4. **Polish.** Architecture notes, benchmark charts, design tradeoffs.

## Build and test

```sh
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

Add `-DOBM_SANITIZE=ON` for an AddressSanitizer and UBSan build.

## Formatting

Code style is defined in `.clang-format` (Google style, 100 columns) and checked in CI.

- **VS Code:** files are formatted on save. Install the recommended C/C++ extension when prompted.
- **Command line:** `cmake --build build --target format` formats everything; `--target format-check` only checks.

## Layout

| Path | Contents |
| --- | --- |
| `include/obm/` | Public headers |
| `src/` | Engine implementation |
| `tests/` | GoogleTest unit tests |
| `bench/` | Benchmarks (step 3) |
| `docs/` | Design notes |
