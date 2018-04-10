#include "Cell.h"
Cell::Cell()
{
    this->col = vec3(0);
    this->occ = 0;
}
Cell Cell::operator+ (const Cell &c)
{
    Cell cell;
    cell.col = this->col + c.col;
    cell.occ = this->occ + c.occ;
    return cell;
}

Cell Cell::operator* (const float x)
{
    Cell cell;
    cell.col = this->col * x;
    cell.occ = this->occ * x;
    return cell;
}

Cell Cell::operator/ (const float x)
{
    Cell cell;
    cell.col = this->col / x;
    cell.occ = this->occ / x;
    return cell;
}