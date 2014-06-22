#ifndef LISTUTILS_H
#define LISTUTILS_H

#include <QVector>

/**
  @brief The OpenTodoList namespace
 */
namespace OpenTodoList {

/**
  @brief Namespace used for utility functions on lists
 */
namespace ListUtils {

/**
  @brief Template function used to find insertion points in lists

  This utility function can be used to find insertion points for list elements.
  It gets an arbitrary container as well as an element to insert. In addition,
  a comparator (in the form of a functor) has to be passed.

  The function will return the index pointing to the first element
  which is not less than (given the lessThen operator).
 */
template<typename T, typename LessThan, typename Container>
int findInsertIndex( const Container &container, const T &newItem, const LessThan &comparator ) {
    for ( int i = 0; i < container.size(); ++i ) {
        if ( comparator( newItem, container.at( i ) ) < 0 ) {
            return i;
        }
    }
    return container.size();
}

}

}

#endif // LISTUTILS_H
