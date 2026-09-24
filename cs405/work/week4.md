PIEFG
 │
 ├── N  → Who plays?
 ├── A  → What actions exist?
 ├── H  → What histories are possible?
 ├── X  → What can be played now?
 ├── P  → Who plays now?
 └── ui → How much does each player get?


              HISTORY h
                   │
                   ▼
             Who plays?
                P(h)
                   │
                   ▼
          What can they do?
                X(h)
                   │
             choose action
                   │
                   ▼
             New history
              (h, a)
                   │
                   ▼
              Game ends?
              /        \
            No          Yes
            │            │
            ↓            ↓
         Continue       u_i


PSNE
 │
 ├── Can contain non-credible threats
 │
 ↓
SPNE
 │
 ├── Must be Nash equilibrium in EVERY subgame
 │
 ↓
Backward Induction
 │
 └── Solve from the end → move backward


BACKIND(h):

    if h ∈ Z:
        return u(h), ∅

    best_utilP(h) = -∞

    for each a ∈ X(h):
        util_at_childP(h) = BACKIND(h,a)

        if util_at_childP(h) > best_utilP(h):
            best_utilP(h) = util_at_childP(h)
            best_actionP(h) = a

    return best_utilP(h), best_actionP(h)


Normal Form Game
      │
      │ sequential games need richer representation
      ↓
PIEFG
      │
      ├── Players N
      ├── Actions A
      ├── Histories H
      ├── Terminal histories Z
      ├── Available actions X(h)
      ├── Player function P(h)
      └── Utilities ui
      │
      ↓
Complete strategies
      │
      ↓
Convert to NFG
      │
      ↓
PSNE
      │
      └── may contain non-credible threats
      ↓
SPNE
      │
      └── rational in every subgame
      ↓
Backward Induction
      │
      └── solve from leaves → root
      ↓
Centipede Game
      │
      └── exposes limitations of SPNE
      ↓
Beliefs → next concepts