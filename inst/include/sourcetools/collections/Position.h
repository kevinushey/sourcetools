#ifndef SOURCE_TOOLS_COLLECTIONS_POSITION_H
#define SOURCE_TOOLS_COLLECTIONS_POSITION_H

#include <cstddef>

namespace sourcetools {
namespace collections {

struct Position
{
  Position() : Position(0, 0) {}

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
} // namespace sourcetools

#endif /* SOURCE_TOOLS_COLLECTIONS_POSITION_H */
