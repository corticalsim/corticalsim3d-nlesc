#ifndef DLLIST_H_
#define DLLIST_H_
#include <list>
#include <array>
#include <memory_resource>

template <class T>
using DLIter = typename std::pmr::list<T>::iterator;

template <class T, size_t granularity = 1024>
class DLList
{
    std::array<T, granularity * sizeof(T)> buffer;
    std::pmr::monotonic_buffer_resource mbr{ buffer.data(), buffer.size() };
    std::pmr::synchronized_pool_resource pool{ &mbr };
    std::pmr::list<T> list_{ &pool };

  public:

    ~DLList() { removeAll(); };

    inline int size() { return list_.size(); }

    inline DLIter<T> begin() { return list_.begin(); }

    inline DLIter<T> end() { return list_.end(); }

    inline DLIter<T> cbegin() { return list_.cbegin(); }

    inline DLIter<T> cend() { return list_.cend(); }

    inline DLIter<T> first() { return list_.begin(); }

    inline DLIter<T> last() { return std::prev(list_.end()); }

    inline DLIter<T> next(DLIter<T> el) { return std::next(el); }

    inline DLIter<T> previous(DLIter<T> el) { return std::prev(el); }

    template <class... Params>
    DLIter<T> create(Params&&... params)
    {
        // Pass any parameters that T's constructors accept.
        const auto it{ list_.emplace_back(std::forward<Params>(params)...) };
        return it;
    };

    DLIter<T> remove(DLIter<T> element)
    {
        // Removes a single element.
        // A check is performed to see if there are any elements in the list.
        // Returns the end iterator if the removed element was the last one,
        // otherwise it returns the element *after* the one that was removed.
        return element == list_.end() ? list_.end() : list_.erase(element);
    };

    void removeAll() { list_.clear(); };

    DLIter<T> import(const DLList<T>& oldList, DLIter<T> element)
    {
        // Import a sequence of elements.
        auto it{ list_.splice(element) };
        oldList.list_.erase(element);
        return it;
    };

    void importSet(const DLList<T>& oldList, DLIter<T> firstElement, DLIter<T> lastElement)
    {
        // This is very inefficient, but it should work as a first approximation.
        // Ideally, this method should use `std::pmr::list::splice()`.
        for (auto current = firstElement; current != lastElement; ++current)
        {
            import(oldList, current);
        }
    };
};

#endif // DLLIST_H_
