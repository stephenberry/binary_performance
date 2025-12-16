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
| Complex Nested Object | 2.3x/2.2x | 2.0x/10.0x | 7.7x/32.1x | 1.3x/2.5x |
| std::vector<double> (10K) | 152.3x/144.5x | 17.5x/37.9x | 156.5x/137.7x | 1.9x/1.5x |
| std::vector<float> (10K) | 229.2x/235.9x | 34.7x/76.6x | 159.5x/301.5x | 1.9x/1.5x |
| std::vector<uint64_t> (10K) | 43.6x/80.6x | 17.1x/35.0x | 155.9x/126.0x | 1.8x/1.5x |
| std::vector<uint32_t> (10K) | 53.8x/87.4x | 34.1x/72.7x | 160.5x/287.3x | 1.9x/1.5x |
| std::vector<uint16_t> (10K) | 94.5x/157.8x | 68.9x/194.5x | 202.8x/684.2x | 1.9x/2.0x |

## Message Sizes

| Test | JSON | BEVE | MessagePack | CBOR | Protobuf |
|------|------|------|-------------|------|----------|
| Complex Nested Object | 616 B | 564 B | 545 B | 545 B | 344 B |
| std::vector<double> (10K) | 219.02 KB | 78.13 KB | 87.89 KB | 87.89 KB | 78.13 KB |
| std::vector<float> (10K) | 124.11 KB | 39.07 KB | 48.83 KB | 48.83 KB | 39.07 KB |
| std::vector<uint64_t> (10K) | 199.23 KB | 78.13 KB | 87.89 KB | 87.89 KB | 78.13 KB |
| std::vector<uint32_t> (10K) | 104.97 KB | 39.07 KB | 48.83 KB | 48.83 KB | 39.07 KB |
| std::vector<uint16_t> (10K) | 56.96 KB | 19.53 KB | 29.26 KB | 29.26 KB | 19.54 KB |

## Detailed Results

### Complex Nested Object

**Iterations:** 1000000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 616 B | 564 B | 545 B | 545 B | 344 B |
| Write Throughput | 1.42 GB/s | 2.97 GB/s | 1.47 GB/s | 370.39 MB/s | 1.38 GB/s |
| Read Throughput | 1.30 GB/s | 2.66 GB/s | 258.61 MB/s | 80.20 MB/s | 645.53 MB/s |

### std::vector<double> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 219.02 KB | 78.13 KB | 87.89 KB | 87.89 KB | 78.13 KB |
| Write Throughput | 1.14 GB/s | 61.87 GB/s | 3.97 GB/s | 444.77 MB/s | 33.31 GB/s |
| Read Throughput | 1.20 GB/s | 61.65 GB/s | 1.83 GB/s | 503.74 MB/s | 40.25 GB/s |

### std::vector<float> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 124.11 KB | 39.07 KB | 48.83 KB | 48.83 KB | 39.07 KB |
| Write Throughput | 865.66 MB/s | 62.47 GB/s | 2.25 GB/s | 489.46 MB/s | 32.20 GB/s |
| Read Throughput | 828.23 MB/s | 61.50 GB/s | 1.00 GB/s | 255.02 MB/s | 40.97 GB/s |

### std::vector<uint64_t> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 199.23 KB | 78.13 KB | 87.89 KB | 87.89 KB | 78.13 KB |
| Write Throughput | 3.51 GB/s | 60.08 GB/s | 3.96 GB/s | 433.63 MB/s | 33.20 GB/s |
| Read Throughput | 1.86 GB/s | 58.88 GB/s | 1.89 GB/s | 525.51 MB/s | 40.21 GB/s |

### std::vector<uint32_t> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 104.97 KB | 39.07 KB | 48.83 KB | 48.83 KB | 39.07 KB |
| Write Throughput | 3.11 GB/s | 62.22 GB/s | 2.28 GB/s | 484.44 MB/s | 32.37 GB/s |
| Read Throughput | 1.87 GB/s | 60.89 GB/s | 1.05 GB/s | 264.86 MB/s | 40.59 GB/s |

### std::vector<uint16_t> (10K)

**Iterations:** 100000

| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |
|--------|------|------|-------------|------|----------|
| Message Size | 56.96 KB | 19.53 KB | 29.26 KB | 29.26 KB | 19.54 KB |
| Write Throughput | 1.95 GB/s | 63.19 GB/s | 1.37 GB/s | 466.73 MB/s | 33.67 GB/s |
| Read Throughput | 1.51 GB/s | 81.57 GB/s | 627.99 MB/s | 178.61 MB/s | 41.05 GB/s |

## Analysis

### Why BEVE Excels at Numeric Arrays

BEVE stores contiguous arrays of trivially copyable types as raw memory blocks, enabling efficient `memcpy` operations. MessagePack and CBOR encode each element individually with type tags, resulting in significant overhead for numeric data. Protobuf (via zpp_bits) also uses memcpy for packed repeated fields, but has additional overhead from varint length prefixes, field tags, and in this benchmark, struct conversion between native C++ types and protobuf-compatible types.
