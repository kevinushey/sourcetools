#ifndef PARSR_COLLECTIONS_POSITION_H
#define PARSR_COLLECTIONS_POSITION_H

#include <cstddef>

namespace parsr {
namespace collections {

struct Position
{
public:

  Position(std::size_t row, std::size_t column)
    : row(row), column(column)
  {}

  friend bool operator <(const Position& lhs, const Position& rhs)
  {
    return
      lhs.row < rhs.row ||
      (lhs.row == rhs.row && lhs.column < rhs.column);
  }

  friend bool operator <=(const Position& lhs, const Position& rhs)
  {
    return
      lhs.row < rhs.row ||
      (lhs.row == rhs.row && lhs.column <= rhs.column);
  }

  friend bool operator ==(const Position& lhs, const Position& rhs)
  {
    return
      lhs.row == rhs.row &&
      lhs.column == rhs.column;
  }

  friend bool operator >(const Position& lhs, const Position& rhs)
  {
    return
      lhs.row > rhs.row ||
      (lhs.row == rhs.row && lhs.column > rhs.column);
  }

  friend bool operator >=(const Position& lhs, const Position& rhs)
  {
    return
      lhs.row > rhs.row ||
      (lhs.row == rhs.row && lhs.column >= rhs.column);
  }

  std::size_t row;
  std::size_t column;

};

} // namespace collections
} // namespace parsr

#endif /* PARSR_COLLECTIONS_POSITION_H */
