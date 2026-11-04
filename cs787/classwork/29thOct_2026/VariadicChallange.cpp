// develop classes Obj, Pred, Fact, Rule, List
// seed ideas below
// use variadic templates

#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
#include <variant>
#include <map>
#include <optional>
using namespace std;

// ---------------------------------------------------------------------------
// Step 1: named building blocks + Fact + FactDB
// ---------------------------------------------------------------------------

// A variable such as X, Y, Z. (Binding/list storage comes in Step 3.)
class VARIABLE {
public:
    string name;
    VARIABLE(string n) : name(n) {}
};

// A constant object such as john, mary, chess.
// Two Obj are "the same" if they are the same C++ object (same address).
class Obj {
public:
    string name;
    Obj(string n) : name(n) {}
};

// A predicate (relation name) such as likes, loves.
class Pred {
public:
    string name;
    Pred(string n) : name(n) {}
};

// A fact: one predicate applied to any number of objects.
//   Fact(likes, john, mary)        -> likes(john, mary)        arity 2
//   Fact(likes, bob, mary, chess)  -> likes(bob, mary, chess)  arity 3
class Fact {
public:
    const Pred* pred;
    vector<const Obj*> args;

    // Variadic constructor: Objs... is a pack of zero or more types.
    template <typename... Objs>
    Fact(const Pred& p, const Objs&... objs);

    void print() const {
        cout << pred->name << "(";
        for (size_t i = 0; i < args.size(); i++) {
            if (i > 0) cout << ", ";
            cout << args[i]->name;
        }
        cout << ")";
    }
};

// The global store of all facts. Everything is static: there is one database.
class FactDB {
public:
    static void add(const Fact& f) { facts.push_back(f); }

    // Read-only access for matching (Step 2) and queries (Step 3).
    static const vector<Fact>& all() { return facts; }

    static void dump() {
        cout << "---- FactDB (" << facts.size() << " facts) ----\n";
        for (const Fact& f : facts) {
            f.print();
            cout << "\n";
        }
    }

private:
    inline static vector<Fact> facts;   // C++17 inline static member
};

// Defined after FactDB so that FactDB::add is visible here.
template <typename... Objs>
Fact::Fact(const Pred& p, const Objs&... objs)
    : pred(&p), args{&objs...}           // pack expansion: {&john, &mary, ...}
{
    // Fold over && : every type in the pack must be Obj (no VARIABLE in a fact).
    static_assert((is_same_v<Objs, Obj> && ...),
                  "Fact arguments after the predicate must all be Obj");
    FactDB::add(*this);                  // a bare Fact(...) statement registers itself
}

// ---------------------------------------------------------------------------
// Step 2: patterns, bindings and matching (unification against one fact)
// ---------------------------------------------------------------------------

// One argument position of a pattern: either a fixed object or a variable.
using Term = variant<const Obj*, const VARIABLE*>;

// Variable -> the object it is currently bound to.
using Bindings = map<const VARIABLE*, const Obj*>;

// A pattern looks like a fact, but may contain variables: likes(X, mary).
class Pattern {
public:
    const Pred* pred;
    vector<Term> terms;

    template <typename... Ts>
    Pattern(const Pred& p, const Ts&... ts)
        : pred(&p), terms{Term{&ts}...}   // each &ts becomes a const Obj* or const VARIABLE*
    {
        static_assert(((is_same_v<Ts, Obj> || is_same_v<Ts, VARIABLE>) && ...),
                      "Pattern arguments must be Obj or VARIABLE");
    }
};

// Try to match pattern `pat` against fact `f`, starting from the bindings in
// `start` (variables that were already fixed earlier).
// Returns the extended bindings on success, or nullopt on failure.
// `start` is never modified, so a failed attempt leaves nothing behind.
optional<Bindings> match(const Pattern& pat, const Fact& f, const Bindings& start = {})
{
    // 1. Same predicate (compare by identity) and same arity.
    if (pat.pred != f.pred) return nullopt;
    if (pat.terms.size() != f.args.size()) return nullopt;

    Bindings b = start;   // work on a copy

    // 2. Compare position by position.
    for (size_t i = 0; i < pat.terms.size(); i++) {
        const Obj* value = f.args[i];

        if (holds_alternative<const Obj*>(pat.terms[i])) {
            // Fixed object in the pattern: must be the very same object.
            if (get<const Obj*>(pat.terms[i]) != value) return nullopt;
        } else {
            const VARIABLE* var = get<const VARIABLE*>(pat.terms[i]);
            auto it = b.find(var);
            if (it == b.end()) {
                b[var] = value;                         // unbound: bind it
            } else if (it->second != value) {
                return nullopt;                         // bound to something else: conflict
            }
            // bound to the same object: consistent, carry on
        }
    }
    return b;
}

void printBindings(const optional<Bindings>& r)
{
    if (!r) { cout << "no match\n"; return; }
    cout << "match {";
    bool first = true;
    for (auto& [var, obj] : *r) {
        cout << (first ? " " : ", ") << var->name << "=" << obj->name;
        first = false;
    }
    cout << " }\n";
}

int main()
{
    VARIABLE X{"X"}, Y{"Y"}, Z{"Z"}, A{"A"};
    Obj john{"john"}, mary{"mary"}, bob{"bob"}, ron{"ron"}, chess{"chess"}, jean{"jean"};
    Pred likes{"likes"}, loves{"loves"};

    Fact (likes, john, mary);
    Fact (likes, bob, mary, chess); //variadic
    Fact (loves, bob, mary);
    Fact (loves, ron, chess, mary);
    Fact (loves, mary, bob, chess); // variadic

    FactDB::dump(); // just dumps all facts to screen -- use for debugging

    // Step 2 checks: match one pattern against one stored fact.
    const vector<Fact>& db = FactDB::all();  // db[0] = likes(john, mary)
                                             // db[1] = likes(bob, mary, chess)
                                             // db[3] = loves(ron, chess, mary)
    cout << "\n---- Step 2: match() ----\n";
    cout << "likes(X, mary)    vs likes(john, mary)        : "; printBindings(match(Pattern(likes, X, mary), db[0]));
    cout << "likes(X, mary)    vs likes(bob, mary, chess)  : "; printBindings(match(Pattern(likes, X, mary), db[1]));
    cout << "loves(X, mary)    vs likes(john, mary)        : "; printBindings(match(Pattern(loves, X, mary), db[0]));
    cout << "likes(X, X)       vs likes(john, mary)        : "; printBindings(match(Pattern(likes, X, X), db[0]));
    cout << "likes(X, mary) {X=bob} vs likes(john, mary)   : "; printBindings(match(Pattern(likes, X, mary), db[0], {{&X, &bob}}));
    cout << "loves(X, Y, Z)    vs loves(ron, chess, mary)  : "; printBindings(match(Pattern(loves, X, Y, Z), db[3]));

    // TODO Step 4: rules (seed typos fixed: missing commas, A and jean declared above)
    // Rule (likes, X, Y, likes, Y, Z, likes, X, Z);   // variadic
    //     //distinguish predicates from objects
    // Rule (likes, X, Y, Z, likes, Z, A, likes, X, A); // variadic rule
    // Rule (likes, X, mary, likes, X, jean);          // variadic rule

    // TODO Step 3: queries
    // Find (likes, X, Y);    // create lists X,Y for corresponding elements in tuples
    // Find (likes, john, X); // first arg is committed, creates a list in X
    // Find (likes, Y, mary); // second arg is committed, creates a list in Y
    // Find (loves, X, Y, Z); // creates lists in X,Y,Z for matching elements forming tuples

    // FactDB::dump();  // will list all facts, including newly added

    // you can also print lists in variables X,Y,Z,A etc after each fact finding
    // statement. have your own print statement like dump();
}
