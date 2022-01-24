#include "node.h"

namespace LemonTea
{

void Node::attempt(Piece& placement, bool hold, PieceType* queue, int queue_size)
{
    this->action.placement = placement;
    this->action.softdrop = !this->state.board.is_above_stack(placement);
    this->action.hold = hold;

    if (hold) {
        if (this->state.hold == PIECE_NONE) {
            ++this->state.next;
        }
        this->state.hold = this->state.current;
    }
    if (this->state.next < queue_size) {
        this->state.current = queue[this->state.next];
        ++this->state.next;
    }
    else {
        this->state.current = PIECE_NONE;
    }

    bool is_t_spin = this->state.board.is_tspin(placement);
    placement.place(this->state.board);
    int line_cleared = this->state.board.clear_line();

    if (line_cleared > 0) {
        ++this->state.ren;
        if (is_t_spin) {
            ++this->state.b2b;
            this->action.lock = (LockType)((int)LOCK_TSPIN_1 + (line_cleared - 1));
        }
        else {
            if (line_cleared == 4) {
                ++this->state.b2b;
            }
            else {
                this->state.b2b = 0;
            }
            if (this->state.board.is_perfect()) {
                this->action.lock = LOCK_PC;
            }
            else {
                this->action.lock = (LockType)((int)LOCK_CLEAR_1 + (line_cleared - 1));
            }
        }
    }
    else {
        this->state.ren = 0;
        this->action.lock = LOCK_NONE;
    }
};

};