#include "TestCollection.h"


#include <cryptoTools/Common/Log.h>
#include <iomanip>
namespace osuCrypto
{

    TestCollection globalTests;


    void TestCollection::add(std::string name, std::function<void()> fn)
    {
        mTests.push_back({ std::move(name), std::move(fn) });
    }
    TestCollection::Result TestCollection::runOne(uint64_t idx)
    {
        bool passed = false;
        Result res = Result::failed;
        int w = int(std::ceil(std::log10(mTests.size())));
        std::cout << std::setw(w) << idx << " - " << Color::Blue << mTests[idx].mName << ColorDefault << std::flush;

        auto start = std::chrono::high_resolution_clock::now();
        try
        {
            mTests[idx].mTest(); std::cout << Color::Green << "  Passed" << ColorDefault;
            res = Result::passed;
        }
        catch (const UnitTestSkipped& e)
        {
            std::cout << Color::Yellow << "  Skipped - " << e.what() << ColorDefault;
            res = Result::skipped;
        }
        catch (const std::exception& e)
        {
            std::cout << Color::Red << "Failed - " << e.what() << ColorDefault;
        }
        auto end = std::chrono::high_resolution_clock::now();



        uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "   " << time << "ms" << std::endl;

        return res;
    }

    TestCollection::Result TestCollection::run(std::vector<u64> testIdxs, u64 repeatCount)
    {
        u64 numPassed(0), total(0), numSkipped(0);

        for (u64 r = 0; r < repeatCount; ++r)
        {
            for (auto i : testIdxs)
            {
                if (repeatCount != 1) std::cout << r << " ";
                auto res = runOne(i);
                numPassed += (res == Result::passed);
                total += (res != Result::skipped);
                numSkipped += (res == Result::skipped);
            }
        }

        if (numPassed == total)
        {
            std::cout << Color::Green << std::endl
                << "=============================================\n"
                << "            All Passed (" << numPassed << ")\n";
            if(numSkipped)
                std::cout << Color::Yellow << "            skipped (" << numSkipped << ")\n";

            std::cout << Color::Green
                << "=============================================" << std::endl << ColorDefault;
            return Result::passed;
        }
        else
        {
            std::cout << Color::Red << std::endl
                << "#############################################\n"
                << "           Failed (" << total - numPassed << ")\n" << Color::Green
                << "           Passed (" << numPassed << ")\n";

            if (numSkipped)
                std::cout << Color::Yellow << "            skipped (" << numSkipped << ")\n";
            
            std::cout << Color::Red
                << "#############################################" << std::endl << ColorDefault;
            return Result::failed;
        }
    }

    TestCollection::Result TestCollection::runAll(uint64_t rp)
    {
        std::vector<u64> v;
        for (u64 i = 0; i < mTests.size(); ++i)
            v.push_back(i);

        return run(v, rp);
    }

    void TestCollection::list()
    {
        int w = int(std::ceil(std::log10(mTests.size())));
        for (uint64_t i = 0; i < mTests.size(); ++i)
        {
            std::cout << std::setw(w) << i << " - " << Color::Blue << mTests[i].mName << std::endl << ColorDefault;
        }
    }


    void TestCollection::operator+=(const TestCollection& t)
    {
        mTests.insert(mTests.end(), t.mTests.begin(), t.mTests.end());
    }
}