# Binary Performance Profiling

Comparing [BEVE](https://github.com/beve-org/beve) (via [Glaze](https://github.com/stephenberry/glaze)) with [MessagePack](https://github.com/msgpack/msgpack-c) for C++ binary serialization performance.

## Results (December 2025)

**Test Environment:**
- Compiler: Apple Clang 17.0.0
- Glaze: 6.2.0
- msgpack-c: 7.0.0
- Iterations: 1,000,000 (complex object), 100,000 (vectors of 10,000 elements)

### Complex Nested Object

| Library | Write Time | Read Time | Message Size |
|---------|------------|-----------|--------------|
| BEVE (Glaze) | 0.187s | 0.211s | 281 bytes |
| MessagePack | 0.378s | 2.095s | 291 bytes |

**BEVE is ~2x faster for writes and ~10x faster for reads, with 3.4% smaller message size.**

### Vector of 10,000 doubles

| Library | Write Time | Read Time | Message Size |
|---------|------------|-----------|--------------|
| BEVE (Glaze) | 0.130s | 0.131s | 80,008 bytes |
| MessagePack | 2.287s | 4.946s | 90,006 bytes |

**BEVE is ~18x faster for writes and ~38x faster for reads.**

### Vector of 10,000 floats

| Library | Write Time | Read Time | Message Size |
|---------|------------|-----------|--------------|
| BEVE (Glaze) | 0.064s | 0.065s | 40,008 bytes |
| MessagePack | 2.244s | 4.915s | 50,005 bytes |

**BEVE is ~35x faster for writes and ~76x faster for reads.**

### Vector of 10,000 uint16_t

| Library | Write Time | Read Time | Message Size |
|---------|------------|-----------|--------------|
| BEVE (Glaze) | 0.032s | 0.026s | 20,006 bytes |
| MessagePack | 2.189s | 4.828s | 30,003 bytes |

**BEVE is ~69x faster for writes and ~186x faster for reads.**

## Why BEVE is faster

BEVE stores contiguous arrays of trivially copyable types (integers, floats) as raw memory blocks, enabling `memcpy` optimization. MessagePack encodes each element individually with type tags, resulting in significant overhead for numeric arrays.

## Building

> Note: [msgpack-c](https://github.com/msgpack/msgpack-c) is expected to be in the same directory as this repository on the branch `cpp_master`.

```bash
git clone https://github.com/msgpack/msgpack-c.git --branch cpp_master
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./binary_perf
```
