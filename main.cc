#include <boost/mpl/and.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/print.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/reverse_fold.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/sizeof.hpp>
//#include <boost/mpl/string.hpp>
#include <boost/mpl/vector.hpp>

#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>

#include "../common/TypeDebugHelper.h"
#include "joins.h"

using namespace boost;
using mpl::_1;
using mpl::_2;

struct ColumnConcept {};
struct FromConcept {};
struct JoinConcept {};
struct WhereConcept {};
struct ExpressionConcept {};
struct SelectConcept {};
struct TableConcept {};

class CTypeABase {};
class CTypeA1;
class CTypeA2;

class CTypeBBase {};
class CTypeB1;
class CTypeB2;

struct SColTypes
{
#if 0
    SqlString
    SqlInt
    SqlLong
    SqlCppType
    SqlBool
#endif
};

template<typename T>
struct DebugT;

#define DEF_COLUMN(name) struct Col##name {}; \
                         template<typename T> struct name \
                         { typedef typename mpl::pair<Col##name, T> type; };

struct TestTable1
{
    DEF_COLUMN(PrimaryKey)
    DEF_COLUMN(Storage)
    DEF_COLUMN(BaseType)
    typedef mpl::vector<ColPrimaryKey, ColStorage, ColBaseType> header;

    // sample table
    // PrimaryKey | Storage |  BaseType
    // -----------+---------+-----------
    // CTypeA1    | int     | CTypeABase
    // CTypeA2    | string  | CTypeABase
    typedef mpl::vector<
        mpl::map3<PrimaryKey<CTypeA1>::type, Storage<int>::type,         BaseType<CTypeABase>::type>,
        mpl::map3<PrimaryKey<CTypeA2>::type, Storage<std::string>::type, BaseType<CTypeABase>::type>
    > type;
};

struct TestTable2
{
    DEF_COLUMN(Tag)
    DEF_COLUMN(Attribute)
    typedef mpl::vector<ColTag, ColAttribute> header;

    typedef mpl::vector<
        mpl::map2<Tag<CTypeA1>::type, Attribute<CTypeB1>::type>,
        mpl::map2<Tag<CTypeA2>::type, Attribute<CTypeB1>::type>,
        mpl::map2<Tag<CTypeA2>::type, Attribute<CTypeB2>::type>
    > type;
};

#if 0
    aggregate functions
    * AVG() - Returns the average value
    * COUNT() - Returns the number of rows
    * FIRST() - Returns the first value
    * LAST() - Returns the last value
    * MAX() - Returns the largest value
    * MIN() - Returns the smallest value
    * SUM() - Returns the sum
#endif
template<typename t_Column>
struct Max
{
    BOOST_MPL_ASSERT((is_same<typename t_Column::concept, ColumnConcept>));
    typedef ColumnConcept concept;
    typedef typename t_Column::type type;

    template<class ColValue>
    struct apply
    {
        typedef ColValue type;
    };
};

template <typename t_Column, template<typename> class t_Wrapper>
struct AddWrapper
{
    BOOST_MPL_ASSERT((is_same<typename t_Column::concept, ColumnConcept>));
    typedef ColumnConcept concept;
    typedef typename t_Column::type type;

    template<class ColValue>
    struct apply
    {
        typedef typename t_Column::template apply<ColValue>::type memberValue;
        typedef t_Wrapper<memberValue> type;
    };
};

template <typename t_Column, template<typename> class t_MplFunction>
struct MplFunction
{
    BOOST_MPL_ASSERT((is_same<typename t_Column::concept, ColumnConcept>));
    typedef ColumnConcept concept;
    typedef typename t_Column::type type;

    template<class ColValue>
    struct apply
    {
        typedef typename t_Column::template apply<ColValue>::type memberValue;
        typedef typename t_MplFunction<memberValue>::type type;
    };
};

template<typename t_Column>
struct Column
{
//    typedef mpl::string<> name;
    typedef ColumnConcept concept;
    typedef t_Column type;

    template<class ColValue>
    struct apply
    {
        typedef ColValue type;
    };
};

// http://stackoverflow.com/questions/3171276/select-multiple-tables-when-one-table-is-empty-in-mysql
// Using two tables in the from clause is functionally equivalent to a cross join
template<typename t_Table1, typename t_Join = mpl_::na>
struct From
{
//    typedef mpl::string<'F','R','O','M'> name;
    typedef FromConcept concept;
//    typedef typename
//        mpl::if_<is_same<t_Join::concept, 
    typedef typename
        mpl::eval_if<
            typename is_same<t_Join, mpl_::na>::type,
            t_Table1,
            mpl::apply<t_Join, t_Table1>
        >::type type;
};

// # JOIN: Return rows when there is at least one match in both tables
// # LEFT JOIN: Return all rows from the left table, even if there are no matches in the right table
// # RIGHT JOIN: Return all rows from the right table, even if there are no matches in the left table
// # FULL JOIN: Return rows when there is a match in one of the tables
// http://www.softwareonline.hu/Article/View.aspx?id=3766
// http://en.wikipedia.org/wiki/Join_%28SQL%29

// Join == InnerJoin
template<typename t_Table2, typename t_Expr>
struct Join
{
//    typedef mpl::string<'F','R','O','M'> name;
    typedef JoinConcept concept;
    typedef typename t_Table2::type table2;

    template<typename t_Table1>
    struct apply
    {
        typedef typename t_Table1::type table1;
        typedef typename t_Table2::type table2;
        typedef typename mpl::inner_join<table1, table2, t_Expr>::type type;
//        typedef table1 type;
    };
};

// explicit cross join
template<typename t_Table2, typename t_Expr>
struct CrossJoin
{
//    typedef mpl::string<'F','R','O','M'> name;
    typedef JoinConcept concept;
    typedef typename t_Table2::type table2;

    template<typename t_Table1>
    struct apply
    {
        typedef typename t_Table1::type table1;
        typedef typename t_Table2::type table2;
        typedef typename mpl::cross_join<table1, table2>::type type;
//        typedef table1 type;
    };
};

template<typename t_Expression>
struct Where
{
//    typedef mpl::string<'W','H','E','R','E'> name;
    typedef WhereConcept concept;
    typedef t_Expression type;
};

struct True
{
    typedef True type;
    typedef ExpressionConcept concept;
    template<class ColMap>
    struct apply
    {
        typedef mpl::true_ type;
    };
};

struct False
{
    typedef False type;
    typedef ExpressionConcept concept;
    template<class ColMap>
    struct apply
    {
        typedef mpl::false_ type;
    };
};

template<typename t_Column, typename t_Value>
struct Equal
{
    typedef t_Column Col1;
    typedef t_Value Col2;

    typedef ExpressionConcept concept;
//    typedef mpl::string<'='> name;
    template<class ColMap>
    struct apply
    {
        BOOST_MPL_ASSERT((typename mpl::has_key<ColMap, t_Column>::type));
        typedef typename mpl::at<ColMap, t_Column>::type TValue;
        typedef typename is_same<t_Value, TValue>::type type;
    };
};

template<typename t_Column, typename t_Value>
struct NotEqual
{
    typedef t_Column Col1;
    typedef t_Value Col2;

    typedef ExpressionConcept concept;
//    typedef mpl::string<'!','='> name;
    template<class ColMap>
    struct apply
    {
        BOOST_MPL_ASSERT((typename mpl::has_key<ColMap, t_Column>::type));
        typedef typename mpl::at<ColMap, t_Column>::type TValue;
        typedef typename mpl::not_<boost::is_same<t_Value, TValue> > type;
    };
};

template<typename Expression1, typename Expression2>
struct Or
{
    BOOST_MPL_ASSERT((is_same<typename Expression1::concept, ExpressionConcept>));
    BOOST_MPL_ASSERT((is_same<typename Expression2::concept, ExpressionConcept>));
//    typedef mpl::string<'O','R'> name;
    typedef ExpressionConcept concept;
    template<class ColMap>
    struct apply
    {
        typedef typename mpl::or_<
            mpl::apply<Expression1, ColMap>,
            mpl::apply<Expression2, ColMap>
        >::type type;
    };
};

template<typename Expression1, typename Expression2>
struct And
{
    BOOST_MPL_ASSERT((is_same<typename Expression1::concept, ExpressionConcept>));
    BOOST_MPL_ASSERT((is_same<typename Expression2::concept, ExpressionConcept>));
//    typedef mpl::string<'A','N','D'> name;
    typedef ExpressionConcept concept;
    template<class ColMap>
    struct apply
    {
        typedef typename mpl::and_<
            mpl::apply<Expression1, ColMap>,
            mpl::apply<Expression2, ColMap>
        >::type type;
    };
};

template<typename t_Column, typename t_From, typename t_Where = Where<True> >
struct Select
{
    BOOST_MPL_ASSERT((is_same<typename t_Column::concept, ColumnConcept>));
    BOOST_MPL_ASSERT((is_same<typename t_From::concept, FromConcept>));
    BOOST_MPL_ASSERT((is_same<typename t_Where::concept, WhereConcept>));
    typedef ColumnConcept concept;

//    typedef mpl::string<'S','E','L','E','C','T'> name;
    typedef typename t_From::type from;
    typedef typename t_Where::type where;
    typedef typename t_Column::type column;

    template<class Res, class Map>
    struct SIfMapContainsItemThenStoreInRes
    {
//        DebugT<Map> debug;
        typedef typename mpl::at<Map, column>::type TRes;
        typedef typename mpl::apply<t_Column, TRes>::type TRes2;
        typedef typename mpl::eval_if<
              mpl::has_key<Map, column>,
              mpl::push_front<Res, mpl::map1<mpl::pair<column, TRes2> > >,
	      Res
	>::type type;
    };

    typedef typename mpl::reverse_fold<
        from,             // from table
        mpl::vector0<>,   // result set
        mpl::eval_if<
//            typename where::template apply<_2>, // OK
//            mpl::apply<where, _2>, // WRONG apply with lambda
            mpl::apply1<where, _2>,
            SIfMapContainsItemThenStoreInRes<_1, _2>,
            _1
        >
    >::type type;
};

class CTypeA1 : public CTypeABase {};
class CTypeA2 : public CTypeABase {};
class CTypeB1 : public CTypeBBase {};
class CTypeB2 : public CTypeBBase {};

void Test()
{
#if 0
    DEBUG_TYPE((TestTable1::type));
    typedef Equal<TestTable1::ColPrimaryKey, CTypeA1> TestExpr;
    typedef mpl::map1<mpl::pair<TestTable1::ColPrimaryKey, CTypeA1> > TestParam;
    typedef mpl::apply<TestExpr, TestParam> TestApply;
    typedef mpl::eval_if<TestApply::type, mpl::true_, mpl::false_>::type TestIf;
    typedef mpl::reverse_fold<TestTable1::type, mpl::vector0<>, _1>::type TestFold;
    DEBUG_TYPE((TestExpr));
    DEBUG_TYPE((TestApply));
    DEBUG_TYPE((TestIf));
#endif
}

template<typename T>
struct my_wrap
{
    typedef T type;
};

int main()
{
//    Test();
    typedef typename mpl::cross_join<TestTable1::type, TestTable2::type>::type Product;
//    typedef typename mpl::joint_view<TestTable1::type, TestTable2::type> Product;

//    DEBUG_TYPE((Product));
    std::cout << "Size1: " << mpl::size<TestTable1::type>::value << std::endl;
    std::cout << "Size2: " << mpl::size<TestTable2::type>::value << std::endl;
    std::cout << "Size: " << mpl::size<Product>::value << std::endl;

    typedef Select<
            AddWrapper<Column<TestTable1::ColStorage>, shared_ptr>,
//            MplFunction<Column<TestTable1::ColStorage>, mpl::sizeof_>,
//            From<TestTable1>,
            From<TestTable1,
//            From<my_wrap<Product>,
               Join<TestTable2, Equal<TestTable1::ColPrimaryKey, TestTable2::ColTag> >
            >
#if 0
            ,
            Where<
                Or<
                    Equal<TestTable1::ColPrimaryKey, CTypeA1>,
                    Equal<TestTable1::ColPrimaryKey, CTypeA2>
                >
           >
#endif
    >::type TResultSet;
    DEBUG_TYPE((TResultSet));
    std::cout << "Size: " << mpl::size<TResultSet>::value << std::endl;
#if 0
    typedef mpl::back<TResultSet>::type TResultRow;
    typedef mpl::at<TResultRow, TestTable1::ColStorage>::type TResultValue;

    DEBUG_TYPE((TResultRow));
    DEBUG_TYPE((TResultValue));
#endif
    //    std::cout << TResultValue::value << std::endl;
//    res = 1;
//    TResultValue res = "test";
}
