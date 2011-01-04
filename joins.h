#ifndef _META_SQL_JOINS__H
#define _META_SQL_JOINS__H
#include <boost/mpl/copy.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/reverse_fold.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>

namespace boost{namespace mpl{

template<typename T>
struct DebugT;

template<typename Item, typename Map>
struct map_inserter
{
    typedef typename mpl::insert<Map, Item>::type type;
};

template<typename Map1, typename Map2>
struct merge_maps
{
    typedef typename
    mpl::reverse_fold<
        Map2,
        Map1,
        map_inserter<_2, _1>
    >::type type;
};

template<typename Result, typename Row>
struct my_push_back
{
    typedef typename push_back<Result, Row>::type type;
};

template<typename Result, typename Row, typename Table2>
struct process_row
{
    typedef typename
    reverse_fold<
        Table2,
        vector0<>,
        my_push_back<_1, merge_maps<Row, _2> >
    >::type type;
};

template<typename Table1, typename Table2>
struct cross_join
{
    typedef typename
    reverse_fold<
        Table1,
        vector0<>,
        joint_view<
            _1,
            process_row<
                _1, // Result
                _2, // RowIter
                Table2
            >
        >
    >::type type;
};

template<typename Table, typename Col, typename ColValue>
struct SGetRowIterFromTable
{
    typedef typename
    find_if<
        Table,
        boost::is_same<
            mpl::at<_1, Col>,
            ColValue
        >
    >::type type;
};

template<typename Table, typename Col, typename ColValue>
struct SIsTableContainsItem
{
    typedef typename
    mpl::not_<
        typename boost::is_same<
            typename end<Table>::type,
            typename SGetRowIterFromTable<Table, Col, ColValue>::type
        >
    >::type type;
};

typedef mpl::map2<
           mpl::pair<int, mpl::int_<3> >,
           mpl::pair<int, mpl::int_<4> >
        > TestRow;

template<typename Table1, typename Table2, typename Expr>
struct inner_join
{
    template<typename Res, typename Row1, typename Row2, typename Col2, typename ColValue>
    struct store_row
    {
        typedef typename merge_maps<Row1, Row2>::type TMergedRows;
        typedef typename
        eval_if<
            boost::is_same<
               typename mpl::at<Row2, Col2>::type,
               ColValue
            >,
            my_push_back<Res, TMergedRows>,
            Res
        >::type type;
    };

    template<typename Row1, typename Col2, typename ColValue>
    struct collect_rows
    {
        typedef typename
        reverse_fold<
            Table2,
            vector0<>,
            store_row<_1, Row1, _2, Col2, ColValue>
        >::type type;
    };

    template<typename Res, typename Row>
    struct SIfMatchThanStore
    {
        typedef typename mpl::at<Row, typename Expr::Col1>::type TItem1;
        typedef typename collect_rows<Row, typename Expr::Col2, TItem1>::type TSubResult;
        typedef typename joint_view<Res, TSubResult>::type type;
    };

    typedef typename
    reverse_fold<
        Table1, vector0<>, SIfMatchThanStore<_1, _2>
    >::type type;
};

}}//exit boost::mpl namespace
#endif
