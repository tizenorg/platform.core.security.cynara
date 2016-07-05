#include <gtest/gtest.h>

#include "SharedStringEx.h"

using namespace Cynara;

namespace {
int getRefCount(const std::string &pattern) {
    return StringStorageEx::getInstance().getRefCount(pattern);
}
} // namespace anonymous

TEST(SharedString, ConstructorDestructor) {
    std::string pattern = "Constructor";
    ASSERT_TRUE(getRefCount(pattern) == -1);
    {
        SharedString ex(pattern);
        ASSERT_TRUE(getRefCount(pattern) == 1);
        SharedString ex2(pattern);
        ASSERT_TRUE(StringStorageEx::getInstance().getRefCount(pattern) == 2);
    }
    StringStorageEx::getInstance().erase(pattern);
    ASSERT_TRUE(getRefCount(pattern) == -1);
}

TEST(SharedString, CopyConstructor) {
    std::string pattern = "CopyConstr";
    ASSERT_TRUE(getRefCount(pattern) == -1);
    {
        SharedString ex(pattern);
        ASSERT_TRUE(getRefCount(pattern) == 1);
        {
            SharedString copy(ex);
            ASSERT_TRUE(getRefCount(pattern) == 2);
        }
        ASSERT_TRUE(getRefCount(pattern) == 1);
    }
    ASSERT_TRUE(getRefCount(pattern) == -1);
}

TEST(SharedString, MoveConstructor) {
    std::string pattern = "MoveConstr";
    ASSERT_TRUE(getRefCount(pattern) == -1);
    {
        SharedString ex(pattern);
        ASSERT_TRUE(getRefCount(pattern) == 1);
        {
            SharedString copy(std::move(ex));
            ASSERT_TRUE(getRefCount(pattern) == 1);
        }
        ASSERT_TRUE(getRefCount(pattern) == -1);
    }
}

TEST(SharedString, CopyOperator) {
    std::string pattern = "CopyOp";
    std::string pattern2 = "COp";
    ASSERT_TRUE(getRefCount(pattern) == -1);
    {
        SharedString ex(pattern);
        ASSERT_TRUE(getRefCount(pattern) == 1);
        {
            SharedString copy(pattern2);
            ASSERT_TRUE(getRefCount(pattern2) == 1);
            copy = ex;
            ASSERT_TRUE(getRefCount(pattern2) == -1);
            ASSERT_TRUE(getRefCount(pattern) == 2);
        }
        ASSERT_TRUE(getRefCount(pattern) == 1);
    }
    ASSERT_TRUE(getRefCount(pattern) == -1);
}

TEST(SharedString, MoveOperator) {
    std::string pattern = "MoveOp";
    std::string pattern2 = "MoveOp2";
    ASSERT_TRUE(getRefCount(pattern) == -1);
    {
        SharedString ex(pattern);
        ASSERT_TRUE(getRefCount(pattern) == 1);
        {
            SharedString copy(pattern2);
            ASSERT_TRUE(getRefCount(pattern2) == 1);
            copy = std::move(ex);
            ASSERT_TRUE(getRefCount(pattern2) == -1);
            ASSERT_TRUE(getRefCount(pattern) == 1);
        }
        ASSERT_TRUE(getRefCount(pattern) == -1);
    }
    ASSERT_TRUE(getRefCount(pattern) == -1);
}

