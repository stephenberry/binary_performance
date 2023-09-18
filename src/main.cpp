
#include <glaze/exceptions/binary_exceptions.hpp>
#include <limits>

#include "glaze/binary/write.hpp"
#include "glaze/core/macros.hpp"
#include "glaze/glaze.hpp"
#include "glaze/glaze_exceptions.hpp"

#define MSGPACK_NO_BOOST
#include "msgpack.hpp"

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

struct fixed_object_t
{
   std::vector<int> int_array;
   std::vector<float> float_array;
   std::vector<double> double_array;

   MSGPACK_DEFINE_MAP(int_array, float_array, double_array);
   GLZ_LOCAL_META(fixed_object_t, int_array, float_array, double_array);
};

struct fixed_name_object_t
{
   std::string name0;
   std::string name1;
   std::string name2;
   std::string name3;
   std::string name4;

   MSGPACK_DEFINE_MAP(name0, name1, name2, name3, name4);
   GLZ_LOCAL_META(fixed_name_object_t, name0, name1, name2, name3, name4);
};

struct nested_object_t
{
   std::vector<std::array<double, 3>> v3s;
   std::string id;

   MSGPACK_DEFINE_MAP(v3s, id);
   GLZ_LOCAL_META(nested_object_t, v3s, id);
};

struct another_object_t
{
   std::string string;
   std::string another_string;
   bool boolean;
   nested_object_t nested_object;

   MSGPACK_DEFINE_MAP(string, another_string, boolean, nested_object);
   GLZ_LOCAL_META(another_object_t, string, another_string, boolean, nested_object);
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
   GLZ_LOCAL_META(obj_t, fixed_object, fixed_name_object, another_object, string_array, string, number, boolean,
                  another_bool);
};

#include <iostream>

#ifdef NDEBUG
static constexpr size_t iterations = 1'000'000;
#else
static constexpr size_t iterations = 100'000;
#endif

struct results
{
   double write{};
   double read{};
};

results eve_test()
{
   std::string buffer{json0};

   obj_t obj{};

   glz::ex::read_json(obj, buffer);

   // write performance
   auto t0 = std::chrono::steady_clock::now();

   for (size_t i = 0; i < iterations; ++i) {
      glz::write_binary(obj, buffer);
   }

   auto t1 = std::chrono::steady_clock::now();

   std::cout << "EVE size: " << buffer.size() << '\n';
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   std::cout << "EVE write performance: " << write << '\n';

   // read performance

   t0 = std::chrono::steady_clock::now();

   for (size_t i = 0; i < iterations; ++i) {
      if (glz::read_binary(obj, buffer)) {
         std::cout << "glaze error!\n";
         break;
      }
   }

   t1 = std::chrono::steady_clock::now();

   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   std::cout << "EVE read performance: " << read << '\n';

   return {write, read};
}

results msgpack_test()
{
   obj_t obj{};
   std::string buffer{json0};
   glz::ex::read_json(obj, buffer);

   // msgpack write performance
   auto t0 = std::chrono::steady_clock::now();

   msgpack::sbuffer packed{};

   for (size_t i = 0; i < iterations; ++i) {
      packed.clear();
      msgpack::pack(packed, obj);
   }

   auto t1 = std::chrono::steady_clock::now();

   // std::cout << "Message: " << std::string_view{packed.data(), packed.size()} << '\n';
   std::cout << "MessagePack size: " << packed.size() << '\n';
   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   std::cout << "MessagePack write performance: " << write << '\n';

   // read performance

   obj = obj_t{};
   t0 = std::chrono::steady_clock::now();

   for (size_t i = 0; i < iterations; ++i) {
      msgpack::object_handle oh = msgpack::unpack(packed.data(), packed.size());
      oh.get().convert(obj);
   }

   t1 = std::chrono::steady_clock::now();

   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   std::cout << "MessagePack read performance: " << read << '\n';

   return {write, read};
}

#include <random>

constexpr auto vector_iterations = 100'000;

results eve_vector_double_test()
{
   constexpr auto N = 10'000;
   std::mt19937_64 gen{};
   std::uniform_real_distribution<double> dist{0.0, (std::numeric_limits<double>::max)()};
   std::vector<double> x(N);
   for (auto& v : x) {
      v = dist(gen);
   }

   // msgpack write performance
   auto t0 = std::chrono::steady_clock::now();

   std::string packed{};

   for (size_t i = 0; i < vector_iterations; ++i) {
      glz::write_binary(x, packed);
   }

   auto t1 = std::chrono::steady_clock::now();

   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   std::cout << "EVE size: " << packed.size() << '\n';
   std::cout << "EVE write performance: " << write << '\n';

   // read performance

   t0 = std::chrono::steady_clock::now();

   for (size_t i = 0; i < vector_iterations; ++i) {
      glz::ex::read_binary(x, packed);
   }

   t1 = std::chrono::steady_clock::now();

   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   std::cout << "EVE read performance: " << read << '\n';

   return {write, read};
}

results msgpack_vector_double_test()
{
   constexpr auto N = 10'000;
   std::mt19937_64 gen{};
   std::uniform_real_distribution<double> dist{0.0, (std::numeric_limits<double>::max)()};
   std::vector<double> x(N);
   for (auto& v : x) {
      v = dist(gen);
   }

   // msgpack write performance
   auto t0 = std::chrono::steady_clock::now();

   msgpack::sbuffer packed{};

   for (size_t i = 0; i < vector_iterations; ++i) {
      packed.clear();
      msgpack::pack(packed, x);
   }

   auto t1 = std::chrono::steady_clock::now();

   const auto write = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   std::cout << "MessagePack size: " << packed.size() << '\n';
   std::cout << "MessagePack write performance: " << write << '\n';

   // read performance

   t0 = std::chrono::steady_clock::now();

   for (size_t i = 0; i < vector_iterations; ++i) {
      msgpack::object_handle oh = msgpack::unpack(packed.data(), packed.size());
      oh.get().convert(x);
   }

   t1 = std::chrono::steady_clock::now();

   const auto read = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;

   std::cout << "MessagePack read performance: " << read << '\n';

   return {write, read};
}

int main()
{
   {
      auto [eve_read, eve_write] = eve_test();
      auto [msgpack_read, msg_pack_write] = msgpack_test();
      std::cout << "Read ratio: " << msgpack_read / eve_read << "X\n";
      std::cout << "Write ratio: " << msg_pack_write / eve_write << "X\n";
   }
   {
      auto [eve_read, eve_write] = eve_vector_double_test();
      auto [msgpack_read, msg_pack_write] = msgpack_vector_double_test();
      std::cout << "Read ratio: " << msgpack_read / eve_read << "X\n";
      std::cout << "Write ratio: " << msg_pack_write / eve_write << "X\n";
   }

   return 0;
}