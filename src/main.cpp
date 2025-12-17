
#include <limits>
#include <type_traits>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <random>

#include "glaze/glaze.hpp"
#include "glaze/beve.hpp"
#include "glaze/cbor.hpp"
#include "glaze/glaze_exceptions.hpp"
#include "glaze/exceptions/binary_exceptions.hpp"

#define MSGPACK_NO_BOOST
#include "msgpack.hpp"

#include "zpp_bits.h"

static constexpr std::string_view json0 = R"(
{
   "fixed_object": {
      "int_array": [0, 1, 2, 3, 4, 5, 6],
      "float_array": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6],
      "double_array": [3288398.238, 233e22, 289e-1, 0.928759872, 0.22222848, 0.1, 0.2, 0.3, 0.4]
   },
   "fixed_name_object": {
      "name0": "James",
      "name1": "Abraham",
      "name2": "Susan",
      "name3": "Frank",
      "name4": "Alicia"
   },
   "another_object": {
      "string": "here is some text",
      "another_string": "Hello World",
      "boolean": false,
      "nested_object": {
         "v3s": [[0.12345, 0.23456, 0.001345],
                  [0.3894675, 97.39827, 297.92387],
                  [18.18, 87.289, 2988.298]],
         "id": "298728949872"
      }
   },
   "string_array": ["Cat", "Dog", "Elephant", "Tiger"],
   "string": "Hello world",
   "number": 3.14,
   "boolean": true,
   "another_bool": false
}
)";

// Structs for BEVE, MessagePack, CBOR
struct fixed_object_t
{
   std::vector<int> int_array;
   std::vector<float> float_array;
   std::vector<double> double_array;

   MSGPACK_DEFINE_MAP(int_array, float_array, double_array);
};

struct fixed_name_object_t
{
   std::string name0;
   std::string name1;
   std::string name2;
   std::string name3;
   std::string name4;

   MSGPACK_DEFINE_MAP(name0, name1, name2, name3, name4);
};

struct nested_object_t
{
   std::vector<std::array<double, 3>> v3s;
   std::string id;

   MSGPACK_DEFINE_MAP(v3s, id);
};

struct another_object_t
{
   std::string string;
   std::string another_string;
   bool boolean;
   nested_object_t nested_object;

   MSGPACK_DEFINE_MAP(string, another_string, boolean, nested_object);
};

struct obj_t
{
   fixed_object_t fixed_object;
   fixed_name_object_t fixed_name_object;
   another_object_t another_object;
   std::vector<std::string> string_array;
   std::string string;
   double number;
   bool boolean;
   bool another_bool;

   MSGPACK_DEFINE_MAP(fixed_object, fixed_name_object, another_object, string_array, string, number, boolean,
                      another_bool);
};

// Protobuf-compatible structs for zpp_bits
namespace pb {

struct fixed_object_t
{
   std::vector<zpp::bits::vsint32_t> int_array;  // signed varint
   std::vector<float> float_array;               // fixed32
   std::vector<double> double_array;             // fixed64

   using serialize = zpp::bits::pb_protocol;
};

struct fixed_name_object_t
{
   std::string name0;
   std::string name1;
   std::string name2;
   std::string name3;
   std::string name4;

   using serialize = zpp::bits::pb_protocol;
};

struct vec3 {
   double x, y, z;
   using serialize = zpp::bits::pb_protocol;
};

struct nested_object_t
{
   std::vector<vec3> v3s;
   std::string id;

   using serialize = zpp::bits::pb_protocol;
};

struct another_object_t
{
   std::string string;
   std::string another_string;
   bool boolean;
   nested_object_t nested_object;

   using serialize = zpp::bits::pb_protocol;
};

struct obj_t
{
   fixed_object_t fixed_object;
   fixed_name_object_t fixed_name_object;
   another_object_t another_object;
   std::vector<std::string> string_array;
   std::string string;
   double number;
   bool boolean;
   bool another_bool;

   using serialize = zpp::bits::pb_protocol;
};

// Convert from regular structs to protobuf structs
inline fixed_object_t to_pb(const ::fixed_object_t& src) {
   fixed_object_t dst;
   dst.int_array.reserve(src.int_array.size());
   for (auto v : src.int_array) dst.int_array.push_back(zpp::bits::vsint32_t{v});
   dst.float_array = src.float_array;
   dst.double_array = src.double_array;
   return dst;
}

inline fixed_name_object_t to_pb(const ::fixed_name_object_t& src) {
   return {src.name0, src.name1, src.name2, src.name3, src.name4};
}

inline nested_object_t to_pb(const ::nested_object_t& src) {
   nested_object_t dst;
   dst.v3s.reserve(src.v3s.size());
   for (const auto& arr : src.v3s) {
      dst.v3s.push_back({arr[0], arr[1], arr[2]});
   }
   dst.id = src.id;
   return dst;
}

inline another_object_t to_pb(const ::another_object_t& src) {
   return {src.string, src.another_string, src.boolean, to_pb(src.nested_object)};
}

inline obj_t to_pb(const ::obj_t& src) {
   return {to_pb(src.fixed_object), to_pb(src.fixed_name_object), to_pb(src.another_object),
           src.string_array, src.string, src.number, src.boolean, src.another_bool};
}

// Convert from protobuf structs back to regular structs
inline ::fixed_object_t from_pb(const fixed_object_t& src) {
   ::fixed_object_t dst;
   dst.int_array.reserve(src.int_array.size());
   for (auto v : src.int_array) dst.int_array.push_back(static_cast<int>(v));
   dst.float_array = src.float_array;
   dst.double_array = src.double_array;
   return dst;
}

inline ::fixed_name_object_t from_pb(const fixed_name_object_t& src) {
   return {src.name0, src.name1, src.name2, src.name3, src.name4};
}

inline ::nested_object_t from_pb(const nested_object_t& src) {
   ::nested_object_t dst;
   dst.v3s.reserve(src.v3s.size());
   for (const auto& v : src.v3s) {
      dst.v3s.push_back({v.x, v.y, v.z});
   }
   dst.id = src.id;
   return dst;
}

inline ::another_object_t from_pb(const another_object_t& src) {
   return {src.string, src.another_string, src.boolean, from_pb(src.nested_object)};
}

inline ::obj_t from_pb(const obj_t& src) {
   return {from_pb(src.fixed_object), from_pb(src.fixed_name_object), from_pb(src.another_object),
           src.string_array, src.string, src.number, src.boolean, src.another_bool};
}

} // namespace pb

#ifdef NDEBUG
static constexpr size_t iterations = 1'000'000;
#else
static constexpr size_t iterations = 100'000;
#endif

struct results
{
   double write{};
   double read{};
   uint64_t size{};
};

// JSON tests (Glaze)
results json_test()
{
   std::string buffer{json0};
   obj_t obj{};
   glz::ex::read_json(obj, buffer);

   auto t0 = std::chrono::steady_clock::now();
   for (size_t i = 0; i < iterations; ++i) {
      [[maybe_unused]] auto ec = glz::write_json(obj, buffer);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   t0 = std::chrono::steady_clock::now();
   for (size_t i = 0; i < iterations; ++i) {
      if (glz::read_json(obj, buffer)) {
         std::cerr << "glaze json error!\n";
         break;
      }
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, buffer.size()};
}

// BEVE tests
results beve_test()
{
   std::string buffer{json0};
   obj_t obj{};
   glz::ex::read_json(obj, buffer);

   auto t0 = std::chrono::steady_clock::now();
   for (size_t i = 0; i < iterations; ++i) {
      [[maybe_unused]] auto ec = glz::write_beve(obj, buffer);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   t0 = std::chrono::steady_clock::now();
   for (size_t i = 0; i < iterations; ++i) {
      if (glz::read_beve(obj, buffer)) {
         std::cerr << "glaze error!\n";
         break;
      }
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, buffer.size()};
}

// MessagePack tests
results msgpack_test()
{
   obj_t obj{};
   std::string buffer{json0};
   glz::ex::read_json(obj, buffer);

   auto t0 = std::chrono::steady_clock::now();
   msgpack::sbuffer packed{};
   for (size_t i = 0; i < iterations; ++i) {
      packed.clear();
      msgpack::pack(packed, obj);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   obj = obj_t{};
   t0 = std::chrono::steady_clock::now();
   for (size_t i = 0; i < iterations; ++i) {
      msgpack::object_handle oh = msgpack::unpack(packed.data(), packed.size());
      oh.get().convert(obj);
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, packed.size()};
}

// CBOR tests (Glaze)
results cbor_test()
{
   obj_t obj{};
   std::string buffer{json0};
   glz::ex::read_json(obj, buffer);

   auto t0 = std::chrono::steady_clock::now();
   std::string packed;
   for (size_t i = 0; i < iterations; ++i) {
      [[maybe_unused]] auto ec = glz::write_cbor(obj, packed);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   obj = obj_t{};
   t0 = std::chrono::steady_clock::now();
   for (size_t i = 0; i < iterations; ++i) {
      if (glz::read_cbor(obj, packed)) {
         std::cerr << "glaze cbor error!\n";
         break;
      }
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, packed.size()};
}

// Protobuf tests (zpp_bits in protobuf mode)
results protobuf_test()
{
   obj_t obj{};
   std::string buffer{json0};
   glz::ex::read_json(obj, buffer);

   // Include conversion in the benchmark for fair comparison
   auto t0 = std::chrono::steady_clock::now();
   std::vector<std::byte> packed;
   for (size_t i = 0; i < iterations; ++i) {
      packed.clear();
      auto pb_obj = pb::to_pb(obj);  // Conversion inside loop
      auto out = zpp::bits::out(packed, zpp::bits::no_size{});
      [[maybe_unused]] auto result = out(pb_obj);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   // Include conversion in the benchmark for fair comparison
   t0 = std::chrono::steady_clock::now();
   for (size_t i = 0; i < iterations; ++i) {
      pb::obj_t pb_obj{};
      auto in = zpp::bits::in(packed, zpp::bits::no_size{});
      [[maybe_unused]] auto result = in(pb_obj);
      [[maybe_unused]] auto native_obj = pb::from_pb(pb_obj);  // Conversion inside loop
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, packed.size()};
}

constexpr auto vector_size = 10'000;
constexpr auto vector_iterations = iterations / 10;

template <class T>
results json_vector_test()
{
   std::mt19937_64 gen{};
   using dist_t = std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>,
                                     std::uniform_int_distribution<T>>;
   dist_t dist{0, (std::numeric_limits<T>::max)()};
   std::vector<T> x(vector_size);
   for (auto& v : x) {
      v = dist(gen);
   }

   auto t0 = std::chrono::steady_clock::now();
   std::string packed{};
   for (size_t i = 0; i < vector_iterations; ++i) {
      [[maybe_unused]] auto ec = glz::write_json(x, packed);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   t0 = std::chrono::steady_clock::now();
   std::vector<T> y;
   for (size_t i = 0; i < vector_iterations; ++i) {
      glz::ex::read_json(y, packed);
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, packed.size()};
}

template <class T>
results beve_vector_test()
{
   std::mt19937_64 gen{};
   using dist_t = std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>,
                                     std::uniform_int_distribution<T>>;
   dist_t dist{0, (std::numeric_limits<T>::max)()};
   std::vector<T> x(vector_size);
   for (auto& v : x) {
      v = dist(gen);
   }

   auto t0 = std::chrono::steady_clock::now();
   std::string packed{};
   for (size_t i = 0; i < vector_iterations; ++i) {
      [[maybe_unused]] auto ec = glz::write_beve(x, packed);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   t0 = std::chrono::steady_clock::now();
   std::vector<T> y;
   for (size_t i = 0; i < vector_iterations; ++i) {
      glz::ex::read_beve(y, packed);
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, packed.size()};
}

template <class T>
results msgpack_vector_test()
{
   std::mt19937_64 gen{};
   using dist_t = std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>,
                                     std::uniform_int_distribution<T>>;
   dist_t dist{0, (std::numeric_limits<T>::max)()};
   std::vector<T> x(vector_size);
   for (auto& v : x) {
      v = dist(gen);
   }

   auto t0 = std::chrono::steady_clock::now();
   msgpack::sbuffer packed{};
   for (size_t i = 0; i < vector_iterations; ++i) {
      packed.clear();
      msgpack::pack(packed, x);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   t0 = std::chrono::steady_clock::now();
   std::vector<T> y;
   for (size_t i = 0; i < vector_iterations; ++i) {
      msgpack::object_handle oh = msgpack::unpack(packed.data(), packed.size());
      oh.get().convert(y);
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, packed.size()};
}

template <class T>
results cbor_vector_test()
{
   std::mt19937_64 gen{};
   using dist_t = std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>,
                                     std::uniform_int_distribution<T>>;
   dist_t dist{0, (std::numeric_limits<T>::max)()};
   std::vector<T> x(vector_size);
   for (auto& v : x) {
      v = dist(gen);
   }

   auto t0 = std::chrono::steady_clock::now();
   std::string packed;
   for (size_t i = 0; i < vector_iterations; ++i) {
      [[maybe_unused]] auto ec = glz::write_cbor(x, packed);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   t0 = std::chrono::steady_clock::now();
   std::vector<T> y;
   for (size_t i = 0; i < vector_iterations; ++i) {
      if (glz::read_cbor(y, packed)) {
         std::cerr << "glaze cbor vector error!\n";
         break;
      }
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, packed.size()};
}

// Protobuf vector wrapper for proper encoding
template <typename T>
struct pb_vector_wrapper {
   std::vector<T> data;
   using serialize = zpp::bits::pb_protocol;
};

template <class T>
results protobuf_vector_test()
{
   std::mt19937_64 gen{};
   using dist_t = std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>,
                                     std::uniform_int_distribution<T>>;
   dist_t dist{0, (std::numeric_limits<T>::max)()};
   std::vector<T> x(vector_size);
   for (auto& v : x) {
      v = dist(gen);
   }

   pb_vector_wrapper<T> wrapper{x};

   auto t0 = std::chrono::steady_clock::now();
   std::vector<std::byte> packed;
   for (size_t i = 0; i < vector_iterations; ++i) {
      packed.clear();
      auto out = zpp::bits::out(packed, zpp::bits::no_size{});
      [[maybe_unused]] auto result = out(wrapper);
   }
   auto t1 = std::chrono::steady_clock::now();
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   t0 = std::chrono::steady_clock::now();
   pb_vector_wrapper<T> y;
   for (size_t i = 0; i < vector_iterations; ++i) {
      auto in = zpp::bits::in(packed, zpp::bits::no_size{});
      [[maybe_unused]] auto result = in(y);
   }
   t1 = std::chrono::steady_clock::now();
   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   return {write, read, packed.size()};
}

struct benchmark_result
{
   std::string name;
   results json;
   results beve;
   results msgpack;
   results cbor;
   results protobuf;
   size_t iterations;
};

std::string format_time(double seconds)
{
   std::ostringstream oss;
   if (seconds < 0.001) {
      oss << std::fixed << std::setprecision(2) << (seconds * 1e6) << " µs";
   }
   else if (seconds < 1.0) {
      oss << std::fixed << std::setprecision(2) << (seconds * 1e3) << " ms";
   }
   else {
      oss << std::fixed << std::setprecision(3) << seconds << " s";
   }
   return oss.str();
}

std::string format_size(uint64_t bytes)
{
   std::ostringstream oss;
   if (bytes < 1024) {
      oss << bytes << " B";
   }
   else if (bytes < 1024 * 1024) {
      oss << std::fixed << std::setprecision(2) << (bytes / 1024.0) << " KB";
   }
   else {
      oss << std::fixed << std::setprecision(2) << (bytes / (1024.0 * 1024.0)) << " MB";
   }
   return oss.str();
}

std::string format_speedup(double baseline, double compared)
{
   std::ostringstream oss;
   double speedup = baseline / compared;
   oss << std::fixed << std::setprecision(1) << speedup << "x";
   return oss.str();
}

std::string format_throughput(uint64_t size, double seconds, size_t iters)
{
   std::ostringstream oss;
   double bytes_per_sec = (size * iters) / seconds;
   if (bytes_per_sec >= 1e9) {
      oss << std::fixed << std::setprecision(2) << (bytes_per_sec / 1e9) << " GB/s";
   }
   else if (bytes_per_sec >= 1e6) {
      oss << std::fixed << std::setprecision(2) << (bytes_per_sec / 1e6) << " MB/s";
   }
   else {
      oss << std::fixed << std::setprecision(2) << (bytes_per_sec / 1e3) << " KB/s";
   }
   return oss.str();
}

void generate_markdown(const std::vector<benchmark_result>& results, const std::string& filename)
{
   std::ofstream out(filename);

   auto now = std::chrono::system_clock::now();
   auto time = std::chrono::system_clock::to_time_t(now);
   std::tm tm = *std::localtime(&time);
   std::ostringstream date_stream;
   date_stream << std::put_time(&tm, "%B %Y");

   out << "# Serialization Performance: JSON vs BEVE vs MessagePack vs CBOR vs Protobuf\n\n";

   out << "Benchmark comparing [JSON](https://www.json.org/), [BEVE](https://github.com/beve-org/beve), and ";
   out << "[CBOR](https://cbor.io/) (via [Glaze](https://github.com/stephenberry/glaze)), ";
   out << "[MessagePack](https://github.com/msgpack/msgpack-c), and ";
   out << "[Protocol Buffers](https://protobuf.dev/) (via [zpp_bits](https://github.com/eyalz800/zpp_bits)).\n\n";

   out << "## Test Environment\n\n";
   out << "| Property | Value |\n";
   out << "|----------|-------|\n";
   out << "| Date | " << date_stream.str() << " |\n";
   out << "| Glaze | " << int(glz::version.major) << "." << int(glz::version.minor) << "." << int(glz::version.patch) << " |\n";
   out << "| msgpack-c | 7.0.0 |\n";
   out << "| zpp_bits (protobuf) | latest |\n";
   out << "| C++ Standard | C++20 |\n";
   out << "| Build | Release (-O3) |\n\n";

   out << "## Speedup vs BEVE (Baseline)\n\n";
   out << "Higher means BEVE is faster by that factor. Format: Write/Read\n\n";
   out << "| Test | JSON | MsgPack | CBOR | Protobuf |\n";
   out << "|------|------|---------|------|----------|\n";
   for (const auto& r : results) {
      out << "| " << r.name << " | ";
      out << format_speedup(r.json.write, r.beve.write) << "/" << format_speedup(r.json.read, r.beve.read) << " | ";
      out << format_speedup(r.msgpack.write, r.beve.write) << "/" << format_speedup(r.msgpack.read, r.beve.read) << " | ";
      out << format_speedup(r.cbor.write, r.beve.write) << "/" << format_speedup(r.cbor.read, r.beve.read) << " | ";
      out << format_speedup(r.protobuf.write, r.beve.write) << "/" << format_speedup(r.protobuf.read, r.beve.read) << " |\n";
   }

   out << "\n## Message Sizes\n\n";
   out << "| Test | JSON | BEVE | MessagePack | CBOR | Protobuf |\n";
   out << "|------|------|------|-------------|------|----------|\n";
   for (const auto& r : results) {
      out << "| " << r.name << " | ";
      out << format_size(r.json.size) << " | ";
      out << format_size(r.beve.size) << " | ";
      out << format_size(r.msgpack.size) << " | ";
      out << format_size(r.cbor.size) << " | ";
      out << format_size(r.protobuf.size) << " |\n";
   }

   out << "\n## Detailed Results\n";

   for (const auto& r : results) {
      out << "\n### " << r.name << "\n\n";
      out << "**Iterations:** " << r.iterations << "\n\n";

      out << "| Metric | JSON | BEVE | MessagePack | CBOR | Protobuf |\n";
      out << "|--------|------|------|-------------|------|----------|\n";

      out << "| Message Size | " << format_size(r.json.size) << " | ";
      out << format_size(r.beve.size) << " | ";
      out << format_size(r.msgpack.size) << " | ";
      out << format_size(r.cbor.size) << " | ";
      out << format_size(r.protobuf.size) << " |\n";

      out << "| Write Throughput | " << format_throughput(r.json.size, r.json.write, r.iterations) << " | ";
      out << format_throughput(r.beve.size, r.beve.write, r.iterations) << " | ";
      out << format_throughput(r.msgpack.size, r.msgpack.write, r.iterations) << " | ";
      out << format_throughput(r.cbor.size, r.cbor.write, r.iterations) << " | ";
      out << format_throughput(r.protobuf.size, r.protobuf.write, r.iterations) << " |\n";

      out << "| Read Throughput | " << format_throughput(r.json.size, r.json.read, r.iterations) << " | ";
      out << format_throughput(r.beve.size, r.beve.read, r.iterations) << " | ";
      out << format_throughput(r.msgpack.size, r.msgpack.read, r.iterations) << " | ";
      out << format_throughput(r.cbor.size, r.cbor.read, r.iterations) << " | ";
      out << format_throughput(r.protobuf.size, r.protobuf.read, r.iterations) << " |\n";
   }

   out << "\n## Analysis\n\n";

   out << "### Why BEVE and CBOR (Glaze) Excel at Numeric Arrays\n\n";
   out << "Both BEVE and CBOR (via Glaze) store contiguous arrays of trivially copyable types as raw memory blocks, ";
   out << "enabling efficient `memcpy` operations. MessagePack encodes each element individually ";
   out << "with type tags, resulting in significant overhead for numeric data. ";
   out << "Protobuf (via zpp_bits) also uses memcpy for packed repeated fields, but has additional overhead from ";
   out << "varint length prefixes, field tags, and in this benchmark, struct conversion between native C++ types and protobuf-compatible types.\n";

   out.close();

   std::cout << "Benchmark results written to: " << filename << "\n";
}

int main()
{
   std::vector<benchmark_result> all_results;

   std::cout << "Running benchmarks...\n\n";

   // Complex object test
   std::cout << "Testing: Complex Nested Object\n";
   auto json_obj = json_test();
   auto beve_obj = beve_test();
   auto msgpack_obj = msgpack_test();
   auto cbor_obj = cbor_test();
   auto protobuf_obj = protobuf_test();
   all_results.push_back({"Complex Nested Object", json_obj, beve_obj, msgpack_obj, cbor_obj, protobuf_obj, iterations});

   // Vector tests
   std::cout << "Testing: std::vector<double> (10,000 elements)\n";
   auto json_double = json_vector_test<double>();
   auto beve_double = beve_vector_test<double>();
   auto msgpack_double = msgpack_vector_test<double>();
   auto cbor_double = cbor_vector_test<double>();
   auto protobuf_double = protobuf_vector_test<double>();
   all_results.push_back({"std::vector<double> (10K)", json_double, beve_double, msgpack_double, cbor_double, protobuf_double, vector_iterations});

   std::cout << "Testing: std::vector<float> (10,000 elements)\n";
   auto json_float = json_vector_test<float>();
   auto beve_float = beve_vector_test<float>();
   auto msgpack_float = msgpack_vector_test<float>();
   auto cbor_float = cbor_vector_test<float>();
   auto protobuf_float = protobuf_vector_test<float>();
   all_results.push_back({"std::vector<float> (10K)", json_float, beve_float, msgpack_float, cbor_float, protobuf_float, vector_iterations});

   std::cout << "Testing: std::vector<uint64_t> (10,000 elements)\n";
   auto json_u64 = json_vector_test<uint64_t>();
   auto beve_u64 = beve_vector_test<uint64_t>();
   auto msgpack_u64 = msgpack_vector_test<uint64_t>();
   auto cbor_u64 = cbor_vector_test<uint64_t>();
   auto protobuf_u64 = protobuf_vector_test<uint64_t>();
   all_results.push_back({"std::vector<uint64_t> (10K)", json_u64, beve_u64, msgpack_u64, cbor_u64, protobuf_u64, vector_iterations});

   std::cout << "Testing: std::vector<uint32_t> (10,000 elements)\n";
   auto json_u32 = json_vector_test<uint32_t>();
   auto beve_u32 = beve_vector_test<uint32_t>();
   auto msgpack_u32 = msgpack_vector_test<uint32_t>();
   auto cbor_u32 = cbor_vector_test<uint32_t>();
   auto protobuf_u32 = protobuf_vector_test<uint32_t>();
   all_results.push_back({"std::vector<uint32_t> (10K)", json_u32, beve_u32, msgpack_u32, cbor_u32, protobuf_u32, vector_iterations});

   std::cout << "Testing: std::vector<uint16_t> (10,000 elements)\n";
   auto json_u16 = json_vector_test<uint16_t>();
   auto beve_u16 = beve_vector_test<uint16_t>();
   auto msgpack_u16 = msgpack_vector_test<uint16_t>();
   auto cbor_u16 = cbor_vector_test<uint16_t>();
   auto protobuf_u16 = protobuf_vector_test<uint16_t>();
   all_results.push_back({"std::vector<uint16_t> (10K)", json_u16, beve_u16, msgpack_u16, cbor_u16, protobuf_u16, vector_iterations});

   std::cout << "\n";

   // Generate markdown report
   generate_markdown(all_results, "results.md");

   // Print summary to console
   std::cout << "\n=== Summary (BEVE speedup vs others, Write/Read) ===\n\n";
   std::cout << std::left << std::setw(30) << "Test"
             << std::setw(14) << "JSON"
             << std::setw(14) << "MsgPack"
             << std::setw(14) << "CBOR"
             << std::setw(14) << "Protobuf" << "\n";
   std::cout << std::string(86, '-') << "\n";

   for (const auto& r : all_results) {
      std::cout << std::left << std::setw(30) << r.name
                << std::setw(14) << (format_speedup(r.json.write, r.beve.write) + "/" + format_speedup(r.json.read, r.beve.read))
                << std::setw(14) << (format_speedup(r.msgpack.write, r.beve.write) + "/" + format_speedup(r.msgpack.read, r.beve.read))
                << std::setw(14) << (format_speedup(r.cbor.write, r.beve.write) + "/" + format_speedup(r.cbor.read, r.beve.read))
                << std::setw(14) << (format_speedup(r.protobuf.write, r.beve.write) + "/" + format_speedup(r.protobuf.read, r.beve.read)) << "\n";
   }

   return 0;
}
