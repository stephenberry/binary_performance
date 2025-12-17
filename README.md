# Binary Serialization Performance

Comparing [JSON](https://www.json.org/), [BEVE](https://github.com/beve-org/beve), [MessagePack](https://github.com/msgpack/msgpack-c), [CBOR](https://cbor.io/), and [Protocol Buffers](https://protobuf.dev/) for C++ binary serialization performance.

- JSON, BEVE, and CBOR use [Glaze](https://github.com/stephenberry/glaze)
- MessagePack uses [msgpack-c](https://github.com/msgpack/msgpack-c)
- Protocol Buffers use [zpp_bits](https://github.com/eyalz800/zpp_bits)

## Test Environment

| Property | Value |
|----------|-------|
| Date | December 2025 |
| Glaze | 6.3.0 |
| msgpack-c | 7.0.0 |
| zpp_bits | latest |
| C++ Standard | C++20 |
| Build | Release (-O3) |

## Speedup vs BEVE (Baseline)

Higher means BEVE is faster by that factor. Format: Write/Read

| Test | JSON | MsgPack | CBOR | Protobuf |
|------|------|---------|------|----------|
| Complex Nested Object | 2.6x/2.2x | 2.2x/10.1x | 1.0x/1.1x | 1.4x/2.5x |
| std::vector\<double\> (10K) | 150.8x/147.6x | 17.5x/37.8x | 1.0x/1.0x | 1.8x/1.5x |
| std::vector\<float\> (10K) | 221.8x/231.6x | 33.1x/73.8x | 1.0x/1.0x | 1.8x/1.5x |
| std::vector\<uint64_t\> (10K) | 45.6x/85.5x | 17.9x/36.5x | 1.0x/1.0x | 1.9x/1.7x |
| std::vector\<uint32_t\> (10K) | 53.8x/88.8x | 34.0x/72.1x | 1.0x/1.0x | 1.9x/1.5x |
| std::vector\<uint16_t\> (10K) | 91.7x/130.8x | 67.6x/164.3x | 1.0x/0.9x | 1.8x/1.6x |

> CBOR uses [RFC 8746](https://datatracker.ietf.org/doc/rfc8746/) typed arrays via Glaze.

## Message Sizes

| Test | JSON | BEVE | MessagePack | CBOR | Protobuf |
|------|------|------|-------------|------|----------|
| Complex Nested Object | 616 B | 564 B | 545 B | 560 B | 344 B |
| std::vector\<double\> (10K) | 219.02 KB | 78.13 KB | 87.89 KB | 78.13 KB | 78.13 KB |
| std::vector\<float\> (10K) | 124.11 KB | 39.07 KB | 48.83 KB | 39.07 KB | 39.07 KB |
| std::vector\<uint64_t\> (10K) | 199.23 KB | 78.13 KB | 87.89 KB | 78.13 KB | 78.13 KB |
| std::vector\<uint32_t\> (10K) | 104.97 KB | 39.07 KB | 48.83 KB | 39.07 KB | 39.07 KB |
| std::vector\<uint16_t\> (10K) | 56.96 KB | 19.53 KB | 29.26 KB | 19.54 KB | 19.54 KB |

## Throughput

### Complex Nested Object (1,000,000 iterations)

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 616 B | 564 B | 545 B | 560 B | 344 B |
| Write Throughput | 1.37 GB/s | 3.29 GB/s | 1.46 GB/s | 3.32 GB/s | 1.39 GB/s |
| Read Throughput | 1.31 GB/s | 2.65 GB/s | 255 MB/s | 2.38 GB/s | 649 MB/s |

### std::vector\<double\> 10K elements (100,000 iterations)

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 219.02 KB | 78.13 KB | 87.89 KB | 78.13 KB | 78.13 KB |
| Write Throughput | 1.14 GB/s | 61.23 GB/s | 3.94 GB/s | 61.77 GB/s | 33.20 GB/s |
| Read Throughput | 1.17 GB/s | 61.34 GB/s | 1.83 GB/s | 61.56 GB/s | 39.80 GB/s |

### std::vector\<float\> 10K elements (100,000 iterations)

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 124.11 KB | 39.07 KB | 48.83 KB | 39.07 KB | 39.07 KB |
| Write Throughput | 846 MB/s | 59.05 GB/s | 2.23 GB/s | 62.08 GB/s | 32.05 GB/s |
| Read Throughput | 821 MB/s | 59.81 GB/s | 1.01 GB/s | 60.01 GB/s | 39.90 GB/s |

### std::vector\<uint64_t\> 10K elements (100,000 iterations)

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 199.23 KB | 78.13 KB | 87.89 KB | 78.13 KB | 78.13 KB |
| Write Throughput | 3.45 GB/s | 61.67 GB/s | 3.87 GB/s | 61.13 GB/s | 31.83 GB/s |
| Read Throughput | 1.83 GB/s | 61.53 GB/s | 1.90 GB/s | 61.77 GB/s | 35.97 GB/s |

### std::vector\<uint32_t\> 10K elements (100,000 iterations)

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 104.97 KB | 39.07 KB | 48.83 KB | 39.07 KB | 39.07 KB |
| Write Throughput | 3.09 GB/s | 61.93 GB/s | 2.27 GB/s | 60.78 GB/s | 32.77 GB/s |
| Read Throughput | 1.83 GB/s | 60.43 GB/s | 1.05 GB/s | 60.60 GB/s | 39.26 GB/s |

### std::vector\<uint16_t\> 10K elements (100,000 iterations)

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 56.96 KB | 19.53 KB | 29.26 KB | 19.54 KB | 19.54 KB |
| Write Throughput | 1.97 GB/s | 61.82 GB/s | 1.37 GB/s | 63.10 GB/s | 33.90 GB/s |
| Read Throughput | 1.53 GB/s | 68.72 GB/s | 627 MB/s | 76.31 GB/s | 43.41 GB/s |

## Analysis

### Why BEVE and CBOR (Glaze) Excel at Numeric Arrays

Both BEVE and CBOR (via Glaze) store contiguous arrays of trivially copyable types as raw memory blocks, enabling efficient `memcpy` operations. This results in throughput exceeding **60 GB/s** for typed arrays.

MessagePack encodes each element individually with type tags, resulting in significant overhead for numeric data.

Protobuf (via zpp_bits) also uses memcpy for packed repeated fields, but has additional overhead from varint length prefixes, field tags, and struct conversion between native C++ types and protobuf-compatible types.

### Message Size Efficiency

- **Protobuf** achieves the smallest size for complex objects (344 B) due to field tag encoding and varint integers
- **BEVE and CBOR** achieve identical sizes for typed arrays via raw memory storage
- **MessagePack** has ~12% overhead for typed arrays due to per-element type tags

## Building

> Note: [msgpack-c](https://github.com/msgpack/msgpack-c) is expected to be in the same directory as this repository on the branch `cpp_master`.

```bash
git clone https://github.com/msgpack/msgpack-c.git --branch cpp_master
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./binary_perf
```

Results are written to `results.md`.
