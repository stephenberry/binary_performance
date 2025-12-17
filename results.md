# Serialization Performance: JSON vs BEVE vs MessagePack vs CBOR vs Protobuf

Benchmark comparing [JSON](https://www.json.org/) and [BEVE](https://github.com/beve-org/beve) (via [Glaze](https://github.com/stephenberry/glaze)), [MessagePack](https://github.com/msgpack/msgpack-c), [CBOR](https://cbor.io/) (via [jsoncons](https://github.com/danielaparker/jsoncons)), and [Protocol Buffers](https://protobuf.dev/) (via [zpp_bits](https://github.com/eyalz800/zpp_bits)).

## Test Environment

| Property | Value |
|----------|-------|
| Date | December 2025 |
| Glaze | 6.2.0 |
| msgpack-c | 7.0.0 |
| jsoncons | 1.5.0 |
| zpp_bits (protobuf) | latest |
| C++ Standard | C++20 |
| Build | Release (-O3) |

## Speedup vs BEVE (Baseline)

Higher means BEVE is faster by that factor. Format: Write/Read

| Test | JSON | MsgPack | CBOR | Protobuf |
|------|------|---------|------|----------|
| Complex Nested Object | 2.3x/2.2x | 2.0x/9.8x | 7.6x/31.8x | 1.3x/2.5x |
| std::vector<double> (10K) | 151.0x/143.3x | 17.7x/38.3x | 126.4x/56.8x | 1.9x/1.5x |
| std::vector<float> (10K) | 234.8x/229.4x | 35.4x/73.9x | 130.1x/106.1x | 1.9x/1.5x |
| std::vector<uint64_t> (10K) | 45.2x/84.1x | 17.4x/36.6x | 126.9x/60.7x | 1.8x/1.5x |
| std::vector<uint32_t> (10K) | 55.7x/91.5x | 34.0x/72.5x | 127.7x/108.4x | 1.9x/1.5x |
| std::vector<uint16_t> (10K) | 93.4x/147.2x | 68.0x/181.2x | 126.1x/270.2x | 1.9x/1.9x |

## Message Sizes

| Test | JSON | BEVE | MessagePack | CBOR | Protobuf |
|------|------|------|-------------|------|----------|
| Complex Nested Object | 616 B | 564 B | 545 B | 545 B | 344 B |
| std::vector<double> (10K) | 219.02 KB | 78.13 KB | 87.89 KB | 78.13 KB | 78.13 KB |
| std::vector<float> (10K) | 124.11 KB | 39.07 KB | 48.83 KB | 39.07 KB | 39.07 KB |
| std::vector<uint64_t> (10K) | 199.23 KB | 78.13 KB | 87.89 KB | 78.13 KB | 78.13 KB |
| std::vector<uint32_t> (10K) | 104.97 KB | 39.07 KB | 48.83 KB | 39.07 KB | 39.07 KB |
| std::vector<uint16_t> (10K) | 56.96 KB | 19.53 KB | 29.26 KB | 19.54 KB | 19.54 KB |

## Detailed Results

### Complex Nested Object

**Iterations:** 1000000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 616 B | 564 B | 545 B | 545 B | 344 B |
| Write Throughput | 1.39 GB/s | 2.92 GB/s | 1.42 GB/s | 371.55 MB/s | 1.39 GB/s |
| Read Throughput | 1.33 GB/s | 2.64 GB/s | 261.12 MB/s | 80.16 MB/s | 643.94 MB/s |

### std::vector<double> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 219.02 KB | 78.13 KB | 87.89 KB | 78.13 KB | 78.13 KB |
| Write Throughput | 1.14 GB/s | 61.39 GB/s | 3.90 GB/s | 485.60 MB/s | 32.87 GB/s |
| Read Throughput | 1.19 GB/s | 60.67 GB/s | 1.78 GB/s | 1.07 GB/s | 39.63 GB/s |

### std::vector<float> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 124.11 KB | 39.07 KB | 48.83 KB | 39.07 KB | 39.07 KB |
| Write Throughput | 853.90 MB/s | 63.11 GB/s | 2.23 GB/s | 485.19 MB/s | 33.72 GB/s |
| Read Throughput | 829.46 MB/s | 59.90 GB/s | 1.01 GB/s | 564.42 MB/s | 39.41 GB/s |

### std::vector<uint64_t> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 199.23 KB | 78.13 KB | 87.89 KB | 78.13 KB | 78.13 KB |
| Write Throughput | 3.43 GB/s | 60.94 GB/s | 3.93 GB/s | 480.17 MB/s | 34.72 GB/s |
| Read Throughput | 1.87 GB/s | 61.58 GB/s | 1.89 GB/s | 1.02 GB/s | 40.42 GB/s |

### std::vector<uint32_t> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 104.97 KB | 39.07 KB | 48.83 KB | 39.07 KB | 39.07 KB |
| Write Throughput | 2.96 GB/s | 61.31 GB/s | 2.25 GB/s | 480.01 MB/s | 32.79 GB/s |
| Read Throughput | 1.79 GB/s | 60.85 GB/s | 1.05 GB/s | 561.46 MB/s | 40.15 GB/s |

### std::vector<uint16_t> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 56.96 KB | 19.53 KB | 29.26 KB | 19.54 KB | 19.54 KB |
| Write Throughput | 1.92 GB/s | 61.63 GB/s | 1.36 GB/s | 488.74 MB/s | 33.09 GB/s |
| Read Throughput | 1.48 GB/s | 74.92 GB/s | 619.27 MB/s | 277.33 MB/s | 39.63 GB/s |

## Analysis

### Why BEVE Excels at Numeric Arrays

BEVE stores contiguous arrays of trivially copyable types as raw memory blocks, enabling efficient `memcpy` operations. MessagePack and CBOR encode each element individually with type tags, resulting in significant overhead for numeric data. Protobuf (via zpp_bits) also uses memcpy for packed repeated fields, but has additional overhead from varint length prefixes, field tags, and in this benchmark, struct conversion between native C++ types and protobuf-compatible types.
