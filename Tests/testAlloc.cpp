#include <string>
#include <iostream>
#include "logduration.h"
#include "test_framework.h"

#include "stupidAllocator.h"

using namespace TCPIP;

void TestBase()
{

  const size_t size = 3;
  stupidAllocator<size> testAlloc;

  auto p0 = testAlloc.allocate();
  auto p1 = testAlloc.allocate();
  auto p2 = testAlloc.allocate();
  auto p3 = testAlloc.allocate();

  ASSERT(p0 != nullptr);
  ASSERT(p1 != nullptr);
  ASSERT(p2 != nullptr);
  ASSERT(p3 == nullptr);

  testAlloc.release(p0);
  auto p4 = testAlloc.allocate();
  auto p5 = testAlloc.allocate();

  ASSERT(p4 != nullptr);
  ASSERT(p5 == nullptr);
}

////////////////////////////////////////////////////////////////////////////

int main()
{
  try
  {
    TestRunner tr;
    RUN_TEST(tr, TestBase);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}