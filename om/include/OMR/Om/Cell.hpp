#ifndef OMR_OM_CELL_HPP_
#define OMR_OM_CELL_HPP_

#include <OMR/Om/Id.hpp>

#include <cstdint>

namespace OMR {
namespace Om {

struct Cell;
struct Map;

class Context;

class MemoryManager;

using CellHeader = std::uintptr_t;

/// A managed blob of memory. All Cells have a one slot header.
struct Cell {
  static constexpr CellHeader FLAGS_MASK = 0xFF;
  static constexpr std::size_t MAP_SHIFT = 8;

  static void construct(Context& cx, Cell* self, Map* map,
                        std::uint8_t flags = 0);

  inline Cell(Map* map, std::uint8_t flags = 0) { set(map, flags); }

  /// Get the map reference.
  Map* map() const { return (Map*)(header >> MAP_SHIFT); }

  /// Set the map reference. No write barrier.
  void map(Map* map) {
    header = (CellHeader(map) << MAP_SHIFT) | (header & FLAGS_MASK);
  }

  std::uint8_t flags() const { return std::uint8_t(header & FLAGS_MASK); }

  /// Set the map and the flags.
  void set(Map* m, std::uint8_t flags) {
    header = (CellHeader(m) << MAP_SHIFT) | (flags & FLAGS_MASK);
  }

  template <typename VisitorT>
  void visit(Context& cx, VisitorT& visitor) {
    visitor.edge(cx, this, (Cell*)map());
  }

  CellHeader header;
};

static_assert(std::is_standard_layout<Cell>::value,
              "Cell must be a StandardLayoutType");

inline void Cell::construct(Context& cx, Cell* self, Map* map,
                            std::uint8_t flags) {
  self->set(map, flags);
}

}  // namespace Om
}  // namespace OMR

#endif  // OMR_OM_CELL_HPP_
