cpp-meta-sql
============

Compile time SQL engine that works with types



I wrote a wrapper around boost-mpl and transformed it to an sql engine. It is a compile time sql thing, not related to normal databases. I have not finished it completely. I have some problem with the interface.

A template meta program run in compile time. So input data always present in compile time that is why I organized the inputs to tables.

Tables are simple:

```
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

    typedef mpl::vector<
        mpl::map2<Tag<CTypeA1>::type, Attribute<CTypeB1>::type>,
        mpl::map2<Tag<CTypeA2>::type, Attribute<CTypeB1>::type>,
        mpl::map2<Tag<CTypeA2>::type, Attribute<CTypeB2>::type>
    > type;
};
```

And you can make queries on it:

```
void TestQuery1()
{
    typedef Select<
            Columns<MplFunction<Column<TestTable1::ColStorage>, mpl::sizeof_> >,
            From<TestTable1,
               Join<TestTable2, Equal<TestTable1::ColPrimaryKey, TestTable2::ColTag> >
            >
    > TResultSet;
    DEBUG_TYPE((TResultSet));
    print_table<TResultSet::type>();
}
```

I have type debug macro and table print template helper.

So the above prints:

```
TResultSet => Select<Columns<MplFunction<Column<TestTable1::ColStorage>, boost::mpl::sizeof_>, mpl_::na>, From<TestTable1, Join<TestTable2, Equal<TestTable1::ColPrimaryKey, TestTable2::ColTag> > >, Where<True> >
+------------------------+
| TestTable1::ColStorage |
+------------------------+
| mpl_::size_t<8ul>      |
| mpl_::size_t<8ul>      |
| mpl_::size_t<4ul>      |
+------------------------+
3 rows in set
```

So this gives back compile time types, not runtime values.

```
void TestQuery()
{
    typedef Select<
            Columns<Column<TestTable1::ColStorage> >,
            From<TestTable1>,
            Where<
                Equal<TestTable1::ColPrimaryKey, CTypeA1>
           >
    > TResultSet;
    DEBUG_TYPE((TResultSet));
    print_table<TResultSet::type>();

    // fetch one row
    typedef mpl::back<TResultSet::type>::type TResultRow;

    // get column ColStorage which is an int, TRes will be an int
    typedef mpl::at<TResultRow, TestTable1::ColStorage>::type TRes;

    // use the "int" type
    TRes testInteger = 9;

    // prints test value
    std::cout << "testInteger:" << testInteger << std::endl;
}
```

this prints:

```
TResultSet => Select<Columns<Column<TestTable1::ColStorage>, mpl_::na>, From<TestTable1, mpl_::na>, Where<Equal<TestTable1::ColPrimaryKey, CTypeA1> > >
+------------------------+
| TestTable1::ColStorage |
+------------------------+
| int                    |
+------------------------+
1 row in set

testInteger:9
```
