#include <vector>
#include <string>
#include <list>
#include <forward_list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
#include <deque>

#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <type_traits>
#include <functional>
#include <utility>
#include <atomic>
#include <thread>
#include <future>
#include <chrono>
#include <iterator>
#include <memory>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "hash_map.hpp"

namespace fefu {

TEST_CASE("constructors") {
    hash_map<std::string, int> a(7);
    REQUIRE(a.bucket_count() == 7);
    REQUIRE(a.size() == 0);
    REQUIRE(a.empty());

    hash_map<std::string, int> b(8);

    std::vector<std::pair<std::string, int>> vec(5);
    vec[0] = {"abc1", 1};
    vec[1] = {"abc2", 2};
    vec[2] = {"abc3", 3};
    vec[3] = {"abc4", 4};
    vec[4] = {"abc5", 5};
    b.insert(vec.begin(), vec.end());

    hash_map<std::string, int> c(b);

    REQUIRE(c == b);
    a = b;
    REQUIRE(a == b);

    REQUIRE(a.bucket("abc1") == b.bucket("abc1"));
    REQUIRE(a.bucket("abc2") == b.bucket("abc2"));
    REQUIRE(a.bucket("abc3") == b.bucket("abc3"));
    REQUIRE(a.bucket("abc4") == b.bucket("abc4"));
    REQUIRE(a.bucket("abc5") == b.bucket("abc5"));

    hash_map<std::string, int> d(std::move(b));
    REQUIRE(d.contains("abc1"));
    REQUIRE(d.contains("abc2"));
    REQUIRE(d.contains("abc3"));
    REQUIRE(d.contains("abc4"));
    REQUIRE(d.contains("abc5"));
}

TEST_CASE("try emplace") {
    hash_map<std::string, int> a(7);
    REQUIRE(a.bucket_count() == 7);
    REQUIRE(a.size() == 0);
    REQUIRE(a.empty());
    a.try_emplace("ttt", 555);
    REQUIRE(a.bucket_count() == 7);
    REQUIRE(a.size() == 1);
    REQUIRE(a.at("ttt") == 555);
    REQUIRE(a.contains("ttt"));
}

TEST_CASE("operator ==") {
    hash_map<std::string, int> a(5);
    hash_map<std::string, int> b(5); //equal
    hash_map<std::string, int> c(5); //same size, different elements
    hash_map<std::string, int> d(6); //same elements, different size
    hash_map<std::string, int> e(5); //same size, different keys

    a.try_emplace("abc1", 5);
    a.try_emplace("abc2", 5);

    a.insert(std::pair<std::string, int>("abc3", 5));
    a.insert(std::pair<std::string, int>("abc4", 5));

    b.try_emplace("abc1", 5);
    b.try_emplace("abc2", 5);

    b.insert(std::pair<std::string, int>("abc3", 5));
    b.insert(std::pair<std::string, int>("abc4", 5));

    c.try_emplace("abc1", 6);
    c.try_emplace("abc2", 5);

    c.insert(std::pair<std::string, int>("abc3", 5));
    c.insert(std::pair<std::string, int>("abc4", 5));

    b.try_emplace("abc1", 5);
    b.try_emplace("abc2", 5);

    b.insert(std::pair<std::string, int>("abc3", 5));
    b.insert(std::pair<std::string, int>("abc4", 5));

    e.try_emplace("abc1", 5);
    e.try_emplace("abc2", 5);

    e.insert(std::pair<std::string, int>("abc3", 5));
    e.insert(std::pair<std::string, int>("abc5", 5));

    REQUIRE(a == b);
    REQUIRE(!(a == c));
    REQUIRE(!(a == d));
    REQUIRE(!(a == e));
}

TEST_CASE("rehash") {
    hash_map<int, int> a(5);
    a.try_emplace(1, 1);
    a.try_emplace(2, 2);
    a.try_emplace(3, 3);
    a.rehash(10);
    REQUIRE(a.contains(1));
    REQUIRE(a.contains(2));
    REQUIRE(a.contains(3));
    REQUIRE(a.size() == 3);
    REQUIRE(a.bucket_count() == 10);
}

TEST_CASE("simple erase") {
    hash_map<int, int> a(5);
    a.try_emplace(1, 1);
    a.try_emplace(2, 2);
    a.try_emplace(3, 3);
    a.erase(a.begin());
    a.insert_or_assign(3, 3);
    int j = 15, i = 17;
    a.insert_or_assign(j, i);
    REQUIRE(a.contains(3));
    a.erase(3);
    REQUIRE(!a.contains(3));
    REQUIRE(a.size() == 2);
}

TEST_CASE("2") {
    hash_map<int, int> a(5);
    REQUIRE(a.size() == 0);
    a.try_emplace(1, 1);
    REQUIRE(a.size() == 1);
    a.try_emplace(2, 2);
    REQUIRE(a.size() == 2);
    a.try_emplace(3, 3);
    REQUIRE(a.size() == 3);
    a.erase(a.begin());
    REQUIRE(a.size() == 2);
    a.insert_or_assign(3, 3);
    REQUIRE(a.size() == 2);
    REQUIRE(a.contains(3));
    a.erase(3);
    REQUIRE(!a.contains(3));
    REQUIRE(a.size() == 1);
}

TEST_CASE("erase") {
    hash_map<std::string, int> a(5);
    a.try_emplace("abc1", 5);
    a.try_emplace("abc2", 5);

    a.erase(a.begin());
    std::string sss = "abc3";
    int kkk = 5;
    a.try_emplace(sss, kkk);
    std::string s = "abc3";
    a.erase("abc3");

    REQUIRE(a.bucket_count() == 5);
    REQUIRE(a.size() == 1);
    REQUIRE(a.bucket("abc3") == a.bucket_count());
}

TEST_CASE("insert iterators") {
    std::vector<std::pair<std::string, int>> vec(5);
    vec[0] = {"abc1", 1};
    vec[1] = {"abc2", 2};
    vec[2] = {"abc3", 3};
    vec[3] = {"abc4", 4};
    vec[4] = {"abc5", 5};
    hash_map<std::string, int> a(1);

    a.insert(vec.begin(), vec.end());

    hash_map<std::string, int> b(vec.begin(), vec.end());

    REQUIRE(b.size() == 5);

    REQUIRE(a.size() == 5);

    REQUIRE(a.at("abc1") == 1);
    REQUIRE(a.at("abc2") == 2);
    REQUIRE(a.at("abc3") == 3);
    REQUIRE(a.at("abc4") == 4);
    REQUIRE(a.at("abc5") == 5);

    REQUIRE(b.at("abc1") == 1);
    REQUIRE(b.at("abc2") == 2);
    REQUIRE(b.at("abc3") == 3);
    REQUIRE(b.at("abc4") == 4);
    REQUIRE(b.at("abc5") == 5);
}

TEST_CASE("merge") {
    std::vector<std::pair<std::string, int>> vec(5);
    vec[0] = {"abc1", 1};
    vec[1] = {"abc2", 2};
    vec[2] = {"abc3", 3};
    vec[3] = {"abc4", 4};
    vec[4] = {"abc5", 5};
    hash_map<std::string, int> a(vec.begin(), vec.end());

    std::vector<std::pair<std::string, int>> vec2(5);
    vec2[0] = {"ab1", 1};
    vec2[1] = {"ab2", 2};
    vec2[2] = {"ab3", 3};
    vec2[3] = {"ab4", 4};
    vec2[4] = {"abc5", 6};
    hash_map<std::string, int> b(vec2.begin(), vec2.end());

    a.merge(b);

    REQUIRE(a.size() == 9);
    REQUIRE(a.contains("abc1"));
    REQUIRE(a.contains("abc2"));
    REQUIRE(a.contains("abc3"));
    REQUIRE(a.contains("abc4"));
    REQUIRE(a.contains("abc5"));
    REQUIRE(a.contains("ab1"));
    REQUIRE(a.contains("ab2"));
    REQUIRE(a.contains("ab3"));
    REQUIRE(a.contains("ab4"));

    std::vector<std::pair<std::string, int>> vec1(5);
    vec1[0] = {"abc1", 1};
    vec1[1] = {"abc2", 2};
    vec1[2] = {"abc3", 3};
    vec1[3] = {"abc4", 4};
    vec1[4] = {"abc5", 5};
    hash_map<std::string, int> c(vec1.begin(), vec1.end());

    REQUIRE(c.size() == 5);

    std::vector<std::pair<std::string, int>> vec3(5);
    vec3[0] = {"ab1", 1};
    vec3[1] = {"ab2", 2};
    vec3[2] = {"ab3", 3};
    vec3[3] = {"ab4", 4};
    vec3[4] = {"abc5", 6};
    hash_map<std::string, int> d(vec3.begin(), vec3.end());

    c.merge(std::move(d));

    REQUIRE(c.size() == 9);
    REQUIRE(c.contains("abc1"));
    REQUIRE(c.contains("abc2"));
    REQUIRE(c.contains("abc3"));
    REQUIRE(c.contains("abc4"));
    REQUIRE(c.contains("abc5"));
    REQUIRE(c.contains("ab1"));
    REQUIRE(c.contains("ab2"));
    REQUIRE(c.contains("ab3"));
    REQUIRE(c.contains("ab4"));

    const std::string rrrr = "ab";
    auto t = c.cend();
    t = c.find(rrrr);
    REQUIRE(t == c.cend());
}

TEST_CASE("allocators") {
    std::vector<std::pair<std::string, int>> vec2(5);
    vec2[0] = {"ab1", 1};
    vec2[1] = {"ab2", 2};
    vec2[2] = {"ab3", 3};
    vec2[3] = {"ab4", 4};
    vec2[4] = {"ab5", 5};
    hash_map<std::string, int> b(vec2.begin(), vec2.end());

    allocator<std::pair<std::string, int>> all(5);

    hash_map<std::string, int> c(b, all);

    REQUIRE(b.getAllocator().x == 10);

    REQUIRE(c.getAllocator().x == 5);

    REQUIRE(!(b == c));

    hash_map<std::string, int> d(all);

    REQUIRE(d.size() == 0);
    REQUIRE(d.getAllocator().x == 5);

    hash_map<std::string, int> e(std::move(b), all);
    REQUIRE(e.getAllocator().x == 5);
    REQUIRE(!(b == e));

    hash_map<std::string, int> f({{"a", 5}});
    hash_map<std::string, int> k(5);
    k = std::move(f);

    REQUIRE(k.size() == 1);
    REQUIRE(k.contains("a"));

    auto rrr = k.max_size();
}

TEST_CASE("erase iterators") {
    std::vector<std::pair<std::string, int>> vec(5);
    vec[0] = {"abc1", 1};
    vec[1] = {"abc2", 2};
    vec[2] = {"abc3", 3};
    vec[3] = {"abc4", 4};
    vec[4] = {"abc5", 5};
    hash_map<std::string, int> a(vec.begin(), vec.end());

    REQUIRE(a.size() == 5);

    a.erase(a.begin(), a.end());

    REQUIRE(a.size() == 0);
}

TEST_CASE("reserve") {
    std::vector<std::pair<std::string, int>> vec(5);
    vec[0] = {"abc1", 1};
    vec[1] = {"abc2", 2};
    vec[2] = {"abc3", 3};
    vec[3] = {"abc4", 4};
    vec[4] = {"abc5", 5};
    hash_map<std::string, int> a(vec.begin(), vec.end());

    REQUIRE(a.max_load_factor() == 1.0);
    a.max_load_factor(0.75);
    REQUIRE(a.max_load_factor() == 0.75);

    a.reserve(10);

    REQUIRE(a.size() == 5);
    REQUIRE(a.bucket_count() == 14);
}

TEST_CASE("operator []") {
    std::vector<std::pair<std::string, int>> vec(5);
    vec[0] = {"abc1", 1};
    vec[1] = {"abc2", 2};
    vec[2] = {"abc3", 3};
    vec[3] = {"abc4", 4};
    vec[4] = {"abc5", 5};
    hash_map<std::string, int> a(vec.begin(), vec.end());

    a["abc1"] = 3;
    const int ddd = a.at("abc1");
    REQUIRE(ddd == 3);
    REQUIRE(a.size() == 5);
    std::string kkk = "abc6";
    a[kkk] = 7;
    REQUIRE(a.size() == 6);
    REQUIRE(a.at("abc6") == 7);
    REQUIRE(a.count("abc6") == 1);
    a.erase(a.find("abc6"));
    REQUIRE(!a.contains("abc6"));
}

TEST_CASE("swap") {
    std::vector<std::pair<std::string, int>> vec(5);
    vec[0] = {"abc1", 1};
    vec[1] = {"abc2", 2};
    vec[2] = {"abc3", 3};
    vec[3] = {"abc4", 4};
    vec[4] = {"abc5", 5};
    hash_map<std::string, int> a(vec.begin(), vec.end());

    std::vector<std::pair<std::string, int>> vec2(4);
    vec2[0] = {"ab1", 1};
    vec2[1] = {"ab2", 2};
    vec2[2] = {"ab3", 3};
    vec2[3] = {"ab4", 4};
    hash_map<std::string, int> b(vec2.begin(), vec2.end());

    a.swap(b);

    REQUIRE(a.size() == 4);
    REQUIRE(a.contains("ab1"));
    REQUIRE(a.contains("ab2"));
    REQUIRE(a.contains("ab3"));
    REQUIRE(a.contains("ab4"));

    REQUIRE(b.size() == 5);
    REQUIRE(b.contains("abc1"));
    REQUIRE(b.contains("abc2"));
    REQUIRE(b.contains("abc3"));
    REQUIRE(b.contains("abc4"));
    REQUIRE(b.contains("abc5"));
}

TEST_CASE("clear") {
    std::vector<std::pair<std::string, int>> vec2(4);
    vec2[0] = {"ab1", 1};
    vec2[1] = {"ab2", 2};
    vec2[2] = {"ab3", 3};
    vec2[3] = {"ab4", 4};
    hash_map<std::string, int> b({{"ab1", 1}, {"ab2", 2}, {"ab3", 3}, {"ab4", 4}});

    REQUIRE(b.size() == 4);
    b.clear();
    REQUIRE(b.empty());
    b.emplace("abc", 1);
    b.clear();
    REQUIRE(b.empty());
}

TEST_CASE("operator =") {
    std::vector<std::pair<std::string, int>> vec2(4);
    vec2[0] = {"ab1", 1};
    vec2[1] = {"ab2", 2};
    vec2[2] = {"ab3", 3};
    vec2[3] = {"ab4", 4};
    hash_map<std::string, int> b({{"ab1", 1}, {"ab2", 2}, {"ab3", 3}, {"ab4", 4}});

    hash_map<std::string, int> a(5);
    a = {{"ab1", 1}, {"ab2", 2}, {"ab3", 3}, {"ab4", 4}};

    REQUIRE(a.size() == b.size());

    REQUIRE(a.contains("ab1"));
    REQUIRE(a.contains("ab2"));
    REQUIRE(a.contains("ab3"));
    REQUIRE(a.contains("ab4"));

    REQUIRE(b.contains("ab1"));
    REQUIRE(b.contains("ab2"));
    REQUIRE(b.contains("ab3"));
    REQUIRE(b.contains("ab4"));
}

struct custom_hash
{
  std::vector<long long unsigned int> rnd;

  custom_hash()
  {
      for (int i = 0; i < 1000000; ++i)
          rnd.push_back(rand());
  }

  long long unsigned int operator()(int i) const
  {
      return rnd[i];
  }
};

template<typename TimePoint>
class time_meter
{
 public:
  using time_point = TimePoint;

 private:
  std::function<time_point()> m_fnow;
  std::function<double(time_point, time_point)> m_fsec;
  time_point m_begin;
  time_point m_stop;
  bool m_stopped;

 public:
  template<typename FNow, typename FSec>
  time_meter(FNow&& now, FSec&& sec) : m_fnow(std::forward<FNow>(now)), m_fsec(std::forward<FSec>(sec)), m_begin(m_fnow()), m_stopped(false) { }

  double seconds() const
  {
      if (m_stopped)
          return m_fsec(m_begin, m_stop);
      return m_fsec(m_begin, m_fnow());
  }

  void restart()
  {
      m_stopped = false;
      m_begin = m_fnow();
  }

  void stop()
  {
      if (m_stopped)
          return;
      m_stop = m_fnow();
      m_stopped = true;
  }

  void start()
  {
      if (!m_stopped)
          return;
      m_stopped = false;
      m_begin += m_fnow() - m_stop;
  }
};

auto create_tm()
{
    using tm_type = time_meter<std::chrono::high_resolution_clock::time_point>;

    static const auto get_sec = [](tm_type::time_point p1, tm_type::time_point p2)
    {
      return static_cast<double>((p2 - p1).count()) / std::chrono::high_resolution_clock::period::den;
    };

    return tm_type(std::chrono::high_resolution_clock::now, get_sec);
}

TEST_CASE("_stress")
{
    auto tm = create_tm();
    hash_map<int, int, custom_hash> m(1000000);
    m.max_load_factor(0.1f);
    for (int i = 0; i < 1000000; ++i)
    {
        m.insert({i, i * 3});
    }
    for (int i = 100; i < 999999; ++i)
    {
        m.erase(i);
    }
    for (int i = 0; i < 1000000; ++i)
    {
        m.insert({i, i * 3});
    }
    for (int i = 0; i < 1000000; ++i)
    {
        m.insert({i, i * 3});
    }
    for (int i = 100; i < 999999; ++i)
    {
        m.erase(i);
    }
    for (int i = 100; i < 999999; ++i)
    {
        m.erase(i);
    }
    for (int i = 0; i < 100; ++i)
        REQUIRE(m[i] == i * 3);
    REQUIRE(m[999999] == 2999997);
    REQUIRE(m.size() == 101);
    for (int i = 0; i < 1000; ++i)
        m.insert({rand() % 1000000, rand()});
    std::cout << "**********\n" << "STRESS TIME = " << tm.seconds() << "\n**********" << std::endl;
}

} //namespace fefu