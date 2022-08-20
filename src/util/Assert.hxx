#ifndef CXX20_ASSERT_HXX
#define CXX20_ASSERT_HXX

class Assert
{
public:
    static void that (bool expressionValue);
    [[noreturn]] static void fail ();
};

#endif
