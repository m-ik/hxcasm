# hxcasm

## description

hxcasm is a simple assembler for the Hack assembly language. The Hack computer
is a 16-bit machine based on an extremely minimal architecture.

## build

```bash
make
```

## usage

```bash
./hxcasm <input> [-o output]
```

## tests

### unit

Dependency: `gtest`

```bash
make test-unit
# run all uni tests
./test/unit/run
# run specific tests
./test/unit/run --gtest_filter="ParseTest.*"
```

## debug 

```bash
# build with debug symbols and ASan
make DEBUG=1
make DEBUG=1 test-unit
# run tests with gdb
ASAN_OPTIONS=abort_on_error=1 gdb --args ./test/unit/run --gtest_filter="ParseTest.*"
```

## coding style

Linux kernel coding style has been adopted. `clang-format` is used to
automatically format the code. 

## sources

* The Elements of Computing Systems, Building a Modern Computer from
  First Principles, Noam Nisan and Shimon Schocken
* http://nand2tetris.org
