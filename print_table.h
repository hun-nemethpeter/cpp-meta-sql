#ifndef __OOG_UTIL_PRINTTABLE_H__
#define __OOG_UTIL_PRINTTABLE_H__
using namespace boost;

template<typename T>
std::string debug_type_name()
{
    std::string ret;
    char* name;
    int status;
    name = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
    ret = name;
    free(name);
    return ret;
};

template<typename T>
struct print_table
{
    typedef std::vector<unsigned> TColumnsWidht;
    typedef std::vector<std::string> TTableHeader;
    typedef std::vector<std::vector<std::string> > TTable;

    struct print_cols
    {
        print_cols(TTable& p_table, TTableHeader& p_tableHeader, unsigned p_rowCount) :
            m_table(p_table), m_tableHeader(p_tableHeader), m_rowCount(p_rowCount) {}
        template<typename U>
        void operator()(U&)
        {
            std::string name = debug_type_name<typename mpl::second<U>::type>();
            m_table[m_rowCount].push_back(name);
            if (m_rowCount == 0)
                m_tableHeader.push_back(debug_type_name<typename mpl::first<U>::type>());
        }
        TTable& m_table;
        TTableHeader& m_tableHeader;
        unsigned m_rowCount;
    };

    struct print_rows
    {
        print_rows(TTable& p_table, TTableHeader& p_tableHeader) :
            m_table(p_table), m_tableHeader(p_tableHeader), m_rowCount(0) {}
        template<typename U>
        void operator()(U&)
        {
            mpl::for_each<U>(print_cols(m_table, m_tableHeader, m_rowCount));
            m_rowCount++;
        }
        TTable& m_table;
        TTableHeader& m_tableHeader;
        unsigned m_rowCount;
    };

    void CalculateColumnWidth()
    {
        for (unsigned i = 0; i < m_tableHeader.size(); ++i)
            m_columnsWidth.push_back(m_tableHeader[i].size());
        for (unsigned i = 0; i < m_table.size(); ++i)
        {
            for (unsigned j = 0; j < m_table[0].size(); ++j)
            {
                if (m_table[i][j].size() > m_columnsWidth[i])
                    m_columnsWidth[i] = m_table[i][j].size();
            }
        }
    }
    void PrintHeaderLine()
    {
        for (unsigned i = 0; i < m_tableHeader.size(); ++i)
        {
            if (i == 0)
                std::cout << "+";
            std::cout << "-";
            for (unsigned j = 0; j < m_columnsWidth[i]; ++j)
                std::cout << "-";
            std::cout << "-+";
        }
        std::cout << "\n";
    }
    void PrintHeader()
    {
        for (unsigned i = 0; i < m_tableHeader.size(); ++i)
        {
            if (i == 0)
                std::cout << "|";
            std::cout << " ";
            unsigned width = m_columnsWidth[i];
            std::cout << std::left << std::setw(width) << m_tableHeader[i];
            std::cout << " |";
        }
        std::cout << "\n";
    }
    void PrintTableValues()
    {
        for (unsigned i = 0; i < m_table.size(); ++i)
        {
            for (unsigned j = 0; j < m_table[0].size(); ++j)
            {
                if (j == 0)
                    std::cout << "|";
                std::cout << " ";
                unsigned width = m_columnsWidth[j];
                std::cout << std::left << std::setw(width) << m_table[i][j];
                std::cout << " |";
            }
            std::cout << "\n";
        }
    }
    void print()
    {
        CalculateColumnWidth();
        PrintHeaderLine();
        PrintHeader();
        PrintHeaderLine();
        PrintTableValues();
        PrintHeaderLine();
        unsigned size = mpl::size<T>::type::value;
        if (size == 1)
            std::cout << size << " row in set";
        else
            std::cout << size << " rows in set";
        std::cout << "\n\n";
    }

    print_table() : m_table(mpl::size<T>::type::value)
    {
        mpl::for_each<T>(print_rows(m_table, m_tableHeader));
        print();
    }
    TTable m_table;
    TTableHeader m_tableHeader;
    TColumnsWidht m_columnsWidth;
};
#endif
