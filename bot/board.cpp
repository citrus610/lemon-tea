#include "board.h"

namespace LemonTea
{

bool Piece::operator == (Piece& other)
{
    return (this->x == other.x) && (this->y == other.y) && (this->type == other.type) && (this->rotation == other.rotation);
};

bool Piece::move_right(Board& board)
{
    if (!board.is_colliding(this->x + 1, this->y, this->type, this->rotation)) {
        ++this->x;
        return true;
    }
    return false;
};

bool Piece::move_left(Board& board)
{
    if (!board.is_colliding(this->x - 1, this->y, this->type, this->rotation)) {
        --this->x;
        return true;
    }
    return false;
};

bool Piece::move_cw(Board& board)
{
    assert(this->type != PIECE_O);
    int8_t srs_index = std::min((int8_t)this->type, (int8_t)1);
    int8_t next_rotation = this->rotation + 1;
    if (next_rotation == 4) {
        next_rotation = 0;
    }
    for (int i = 0; i < 5; ++i) {
        int8_t offset_x = SRS_LUT[srs_index][this->rotation][i][0] - SRS_LUT[srs_index][next_rotation][i][0];
        int8_t offset_y = SRS_LUT[srs_index][this->rotation][i][1] - SRS_LUT[srs_index][next_rotation][i][1];
        if (!board.is_colliding(this->x + offset_x, this->y + offset_y, this->type, (PieceRotation)next_rotation)) {
            this->x += offset_x;
            this->y += offset_y;
            this->rotation = (PieceRotation)next_rotation;
            return true;
        }
    }
    return false;
};

bool Piece::move_ccw(Board& board)
{
    assert(this->type != PIECE_O);
    assert(this->type != PIECE_O);
    int8_t srs_index = std::min((int8_t)this->type, (int8_t)1);
    int8_t next_rotation = this->rotation - 1;
    if (next_rotation == -1) {
        next_rotation = 3;
    }
    for (int i = 0; i < 5; ++i) {
        int8_t offset_x = SRS_LUT[srs_index][this->rotation][i][0] - SRS_LUT[srs_index][next_rotation][i][0];
        int8_t offset_y = SRS_LUT[srs_index][this->rotation][i][1] - SRS_LUT[srs_index][next_rotation][i][1];
        if (!board.is_colliding(this->x + offset_x, this->y + offset_y, this->type, (PieceRotation)next_rotation)) {
            this->x += offset_x;
            this->y += offset_y;
            this->rotation = (PieceRotation)next_rotation;
            return true;
        }
    }
    return false;
};

bool Piece::move_down(Board& board)
{
    if (!board.is_colliding(this->x, this->y - 1, this->type, this->rotation)) {
        --this->y;
        return true;
    }
    return false;
};

void Piece::move_drop(Board& board)
{
    this->y -= board.get_drop_distance(*this);
};

void Piece::place(Board& board)
{
    for (int i = 0; i < 4; ++i) {
        int8_t cell_x = PIECE_LUT[this->type][this->rotation][i][0] + this->x;
        int8_t cell_y = PIECE_LUT[this->type][this->rotation][i][1] + this->y;
        board.data[cell_x] |= (1ULL << cell_y);
    }
};

void Piece::normalize()
{
    switch (this->type)
    {
    case PIECE_I:
        switch (this->rotation)
        {
        case PIECE_UP:
            break;
        case PIECE_RIGHT:
            break;
        case PIECE_DOWN:
            this->rotation = PIECE_UP;
            --this->x;
            break;
        case PIECE_LEFT:
            this->rotation = PIECE_RIGHT;
            ++this->y;
            break;
        default:
            break;
        }
        break;
    case PIECE_S:
        switch (rotation)
        {
        case PIECE_UP:
            break;
        case PIECE_RIGHT:
            break;
        case PIECE_DOWN:
            this->rotation = PIECE_UP;
            --this->y;
            break;
        case PIECE_LEFT:
            this->rotation = PIECE_RIGHT;
            --this->x;
            break;
        default:
            break;
        }
        break;
    case PIECE_Z:
        switch (rotation)
        {
        case PIECE_UP:
            break;
        case PIECE_RIGHT:
            break;
        case PIECE_DOWN:
            this->rotation = PIECE_UP;
            --this->y;
            break;
        case PIECE_LEFT:
            this->rotation = PIECE_RIGHT;
            --this->x;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
};

void Piece::mirror()
{
    switch (type)
    {
    case PIECE_I:
        switch (rotation)
        {
        case PIECE_UP:
            rotation = PIECE_DOWN;
            ++x;
            break;
        case PIECE_RIGHT:
            rotation = PIECE_LEFT;
            --y;
            break;
        default:
            break;
        }
        break;
    case PIECE_S:
        switch (rotation)
        {
        case PIECE_UP:
            rotation = PIECE_DOWN;
            ++y;
            break;
        case PIECE_RIGHT:
            rotation = PIECE_LEFT;
            ++x;
            break;
        default:
            break;
        }
        break;
    case PIECE_Z:
        switch (rotation)
        {
        case PIECE_UP:
            rotation = PIECE_DOWN;
            ++y;
            break;
        case PIECE_RIGHT:
            rotation = PIECE_LEFT;
            ++x;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
};

Piece Piece::get_normalize()
{
    Piece copy = *this;
    copy.normalize();
    return copy;
};

Piece Piece::get_mirror()
{
    Piece copy = *this;
    copy.mirror();
    return copy;
};

uint64_t& Board::operator [] (int index)
{
    assert(index > -1 && index < 10);
    return this->data[index];
};

bool Board::operator == (Board& other)
{
    for (int i = 0; i < 10; ++i) {
        if (this->data[i] != other.data[i]) {
            return false;
        }
    }
    return true;
};

void Board::get_height(int height[10])
{
    for (int i = 0; i < 10; ++i) {
        height[i] = 64 - std::countl_zero(this->data[i]);
    }
};

int Board::get_drop_distance(Piece& piece)
{
    int result = 64;
    for (int i = 0; i < 4; ++i) {
        int cell_x = piece.x + PIECE_LUT[piece.type][piece.rotation][i][0];
        int cell_y = piece.y + PIECE_LUT[piece.type][piece.rotation][i][1];
        int cell_distance = cell_y - 64 + std::countl_zero(this->data[cell_x] & ((1ULL << cell_y) - 1));
        result = std::min(result, cell_distance);
    }
    return result;
};

uint64_t Board::get_mask()
{
    uint64_t result = this->data[0];
    for (int i = 1; i < 10; ++i) {
        result &= this->data[i];
    }
    return result;
};

bool Board::is_occupied(const int8_t& x, const int8_t& y)
{
    if (x < 0 || x > 9 || y < 0 || y > 39) {
        return true;
    }
    return ((this->data[x] >> y) & 1) != 0;
};

bool Board::is_colliding(const int8_t& x, const int8_t& y, const PieceType& type, const PieceRotation& rotation)
{
    for (int i = 0; i < 4; ++i) {
        int8_t cell_x = PIECE_LUT[type][rotation][i][0] + x;
        int8_t cell_y = PIECE_LUT[type][rotation][i][1] + y;
        if (is_occupied(cell_x, cell_y)) {
            return true;
        }
    }
    return false;
};

bool Board::is_colliding(Piece& piece)
{
    for (int i = 0; i < 4; ++i) {
        int8_t cell_x = PIECE_LUT[piece.type][piece.rotation][i][0] + piece.x;
        int8_t cell_y = PIECE_LUT[piece.type][piece.rotation][i][1] + piece.y;
        if (is_occupied(cell_x, cell_y)) {
            return true;
        }
    }
    return false;
};

bool Board::is_above_stack(Piece& piece)
{
    for (int i = 0; i < 4; ++i) {
        int8_t cell_x = PIECE_LUT[piece.type][piece.rotation][i][0] + piece.x;
        int8_t cell_y = PIECE_LUT[piece.type][piece.rotation][i][1] + piece.y;
        if (cell_y < 64 - std::countl_zero(this->data[cell_x])) {
            return false;
        }
    }
    return true;
};

bool Board::is_tspin(Piece& piece)
{
    if (piece.type == PIECE_T) {
        switch (piece.rotation)
        {
        case PIECE_UP:
            return this->is_occupied(piece.x + 1, piece.y + 1) && this->is_occupied(piece.x - 1, piece.y + 1) && (this->is_occupied(piece.x + 1, piece.y - 1) || this->is_occupied(piece.x - 1, piece.y - 1));
            break;
        case PIECE_RIGHT:
            return this->is_occupied(piece.x + 1, piece.y + 1) && (this->is_occupied(piece.x + 1, piece.y - 1) + this->is_occupied(piece.x - 1, piece.y - 1) + this->is_occupied(piece.x - 1, piece.y + 1) >= 2);
            break;
        case PIECE_DOWN:
            return this->is_occupied(piece.x - 1, piece.y - 1) && this->is_occupied(piece.x + 1, piece.y - 1) && (this->is_occupied(piece.x - 1, piece.y + 1) || this->is_occupied(piece.x + 1, piece.y + 1));
            break;
        case PIECE_LEFT:
            return this->is_occupied(piece.x - 1, piece.y + 1) && (this->is_occupied(piece.x - 1, piece.y - 1) + this->is_occupied(piece.x + 1, piece.y - 1) + this->is_occupied(piece.x + 1, piece.y + 1) >= 2);
            break;
        default:
            return false;
            break;
        }
    }
    return false;
};

bool Board::is_perfect()
{
    bool result = true;
    for (int i = 0; i < 10; ++i) {
        if (this->data[i] != 0) {
            result = false;
            break;
        }
    }
    return result;
};

int Board::clear_line()
{
    uint64_t mask = this->get_mask();
    if (mask == 0) {
        return 0;
    }
    int mask_tzcnt = std::countr_zero(mask);
    mask = mask >> mask_tzcnt;
    for (int i = 0; i < 10; ++i) {
        uint64_t low_part = this->data[i] & ((1ULL << mask_tzcnt) - 1);
        uint64_t high_part = this->data[i] >> mask_tzcnt;
        switch (mask)
        {
        case 0b0001:
            high_part = high_part >> 1;
            break;
        case 0b0011:
            high_part = high_part >> 2;
            break;
        case 0b0111:
            high_part = high_part >> 3;
            break;
        case 0b1111:
            high_part = high_part >> 4;
            break;
        case 0b0101:
            high_part = ((high_part >> 1) & 0b0001) | ((high_part >> 3) << 2);
            break;
        case 0b1001:
            high_part = ((high_part >> 1) & 0b0011) | ((high_part >> 4) << 2);
            break;
        case 0b1011:
            high_part = ((high_part >> 2) & 0b0001) | ((high_part >> 4) << 1);
            break;
        case 0b1101:
            high_part = ((high_part >> 1) & 0b0001) | ((high_part >> 4) << 1);
            break;
        default:
            return -1;
            break;
        }
        this->data[i] = low_part | (high_part << mask_tzcnt);
    }
    return std::popcount(mask);
};

};