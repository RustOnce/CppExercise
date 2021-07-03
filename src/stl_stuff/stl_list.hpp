template<typename element, template<typename element, class alloc = std::allocator<element> > typename container>
void SortList(container<element>& l1)
{
    std::cout << "Univeral version." << std::endl;
};

template<typename element>
void SortList(std::list<element>& l1)
{
    std::cout << "std::list version." << std::endl;
    std::list<element> carry;
    std::list<element> counter[64];

    int fill = 0;
    while(!l1.empty())
    {
        carry.splice(carry.begin(), l1, l1.begin());
        int i = 0;
        while(i < fill && !counter[i].empty())
        {
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }
        carry.swap(counter[i]);
        if(i == fill) fill++;
    }
    for(int i = 1;i < fill;i++)
    {
        counter[i].merge(counter[i - 1]);
    }
    l1.swap(counter[fill - 1]);
}