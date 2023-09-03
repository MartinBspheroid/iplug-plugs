#pragma once 
#include "IPlugConstants.h"
#include "IGraphics.h"
#include <vector>
#include <numeric>

using namespace iplug;
using namespace igraphics;


class SA_Grid
{
  
public:
  //constructor for grid where columns are defined by fractions and
  // rows are defined by number of divisions
  // this with result in even distribution in vertical axis, but
  // you are free to define horizontal distrubution
  SA_Grid(IRECT _baseRect, std::vector<float> _columns, const int num_rows) : columns(_columns), base(_baseRect)
  { 
    for (size_t i = 0; i < num_rows; i++)
    {
      rows.push_back(1.0f / num_rows);
    }
  }
  //constructor for grid where rows are defined by fractions and
// columns are defined by number of divisions
// this with result in even distribution in horiyontal axis, but
  // you are free to define verital distrubution
  SA_Grid(IRECT _baseRect,  int num_columns, std::vector<float> _rows) : rows(_rows), base(_baseRect)
  {
    for (size_t i = 0; i < num_columns; i++)
    {
      columns.push_back(1.0f / num_columns);
    }
  }
  //constructor for grid where both axis are defined by number of divisions
  // this with result in even distribution in both axes
  SA_Grid(IRECT _baseRect, int num_columns, int num_rows) : base(_baseRect)
  {
    for (size_t i = 0; i < num_columns; i++)
    {
      columns.push_back(1.0f / num_columns);
    }
    for (size_t i = 0; i < num_rows; i++)
    {
      rows.push_back(1.0f / num_rows);
    }
   
  }
  //constructor for grid where both axis are defined by fractions
  // use this used for fine grained definitions of divisions
  SA_Grid(IRECT _baseRect, std::vector<float> _columns, std::vector<float> _rows) :  columns(_columns), rows(_rows), base(_baseRect)
  {}

  // base function for getting area of grid
  // parameters are index offset inside grid, specified at grid construction
  IRECT getArea(const int left, const int top, const int right, const int bottom) {
    try
    {
      float w = base.W();
      float h = base.H();

      float _top = std::accumulate(rows.begin(), rows.begin() + top, 0.0f) * h;
      float _bottom = std::accumulate(rows.end() - (rows.size() - bottom), rows.end(), 0.0f) * -h;

      float _left = std::accumulate(columns.begin(), columns.begin() + left, 0.0f) * w;
      float _right = std::accumulate(columns.end() - (columns.size() - right), columns.end(), 0.0f) * -w;

      return base.GetAltered(_left, _top, _right, _bottom);
    }
    catch (const std::exception& e)
    {
      DBGMSG(e.what());
    }
   
  }
  //return vector<IRECT> of all cells in area defined by _row_
// columnStart & rowStart (x,y)
// columnEnd & rowEnd (x,y)
  std::vector<IRECT> getCellsFromArea(const int columnStart, const int rowStart, const int columnEnd, const int rowEnd) {

    std::vector<IRECT> cells;
    for (size_t y = columnStart; y< columnEnd; y++)
    {
      for (size_t x = rowStart; x < rowEnd; x++)
      {
        cells.push_back(getArea(x, y, x + 1, y+ 1));
      }
    }
    return cells;
  }

  //return vector<IRECT> of all cells in row of index _row_
  // start and end offset is zero by default
  std::vector<IRECT> getRowCells(const int row, const int startOffset = 0, const int endOffset = 0) {

    std::vector<IRECT> rowCells;
    for (size_t i = startOffset; i < columns.size() - endOffset; i++)
    {
      
      rowCells.push_back(getArea(i, row, i + 1, row + 1));
    }
    return rowCells;
  }
  //return vector<IRECT> of all cells in column of index _column_
  // start and end offset is zero by default
  std::vector<IRECT> getComlumnCells(const int column, const int startOffset = 0, const int endOffset = 0) {

    std::vector<IRECT> columnCells;
    for (size_t i = startOffset; i < rows.size() - endOffset; i++)
    {
      columnCells.push_back(getArea(column, i, column + 1, i+1));
    }
    return columnCells;
  }
  // return vector<IRECT> of all cells in current grid
  //starting from top left going all the way to bottom right
  std::vector<IRECT> getAllCells() {
    std::vector<IRECT> cells;
    for (size_t y = 0; y < rows.size(); y++)
    {
      for (size_t x = 0; x < columns.size(); x++)
      {
        cells.push_back(getArea(x, y, x + 1, y + 1));
      }
    }
    return cells;
  }

  
  // return row as single IRECT, with option to specify start and end offset
  IRECT getRowAsArea(const int rowIndex, const int startOffset = 0,  const int endOffset = 0) {
    return getArea(startOffset, rowIndex, columns.size() - endOffset, rowIndex + 1);
  }
  // return column as single IRECT, with option to specify start and end offset
  IRECT getColumnAsArea(const int columnIndex, const int startOffset= 0, const int endOffset = 0 ) {
    return getArea(columnIndex, startOffset, columnIndex + 1, rows.size() - endOffset);

  }
  // get amount of columns in current grid
  const int columnsSize() {
    return columns.size();
  }
  // get amount of rows in current grid
  const int rowsSize() {
    return rows.size();
  }
  // get total amount of cells in current grid
  const int size() {
    return columns.size() * rows.size();
  }
  ~SA_Grid()
  {
  }

private:
  std::vector<float> columns, rows;
  IRECT base;
};

