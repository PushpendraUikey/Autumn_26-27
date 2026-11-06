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
#include <algorithm>
#include <stdexcept>
using namespace std;

// ---------------------------------------------------------------------------
// Step 1: named building blocks + Fact + FactDB
// ---------------------------------------------------------------------------

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

// A list of objects: the answers a variable collects from a Find (Step 3).
class List {
public:
    vector<const Obj*> items;

    void clear() { items.clear(); }
    void add(const Obj* o) { items.push_back(o); }

    void print() const {
        cout << "[";
        for (size_t i = 0; i < items.size(); i++) {
            if (i > 0) cout << ", ";
            cout << items[i]->name;
        }
        cout << "]";
    }
};

// A variable such as X, Y, Z.
// `values` is mutable: the variable's identity (name, address) never changes,
// but Find is allowed to refill its answer list even through a const VARIABLE*.
class VARIABLE {
public:
    string name;
    mutable List values;

    VARIABLE(string n) : name(n) {}

    void print() const {
        cout << name << " = ";
        values.print();
        cout << "\n";
    }
};

// A fact: one predicate applied to any number of objects.
//   Fact(likes, john, mary)        -> likes(john, mary)        arity 2
//   Fact(likes, bob, mary, chess)  -> likes(bob, mary, chess)  arity 3
class Fact {
public:
    const Pred* pred;
    vector<const Obj*> args;
    bool derived = false;    // true if produced by a Rule (Step 4), not written by the user

    // Variadic constructor: Objs... is a pack of zero or more types.
    template <typename... Objs>
    Fact(const Pred& p, const Objs&... objs);

    // Plain constructor used by rules (Step 4). Does NOT register itself:
    // the rule engine decides whether the fact is new before adding it.
    Fact(const Pred* p, vector<const Obj*> a, bool d) : pred(p), args(a), derived(d) {}

    // Same predicate and same objects in the same order.
    bool operator==(const Fact& o) const { return pred == o.pred && args == o.args; }

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

    static bool contains(const Fact& f) {
        return find(facts.begin(), facts.end(), f) != facts.end();   // uses Fact::operator==
    }

    // Read-only access for matching (Step 2) and queries (Step 3).
    static const vector<Fact>& all() { return facts; }

    static void dump() {
        cout << "---- FactDB (" << facts.size() << " facts) ----\n";
        for (const Fact& f : facts) {
            f.print();
            cout << (f.derived ? "   <- derived by a rule" : "") << "\n";
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

    // Plain constructor used by Rule (Step 4), which builds patterns piece by piece.
    Pattern(const Pred* p, vector<Term> t) : pred(p), terms(t) {}

    void print() const {
        cout << pred->name << "(";
        for (size_t i = 0; i < terms.size(); i++) {
            if (i > 0) cout << ", ";
            // Both alternatives have a `name`, so one generic lambda prints either.
            visit([](auto* x) { cout << x->name; }, terms[i]);
        }
        cout << ")";
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

// ---------------------------------------------------------------------------
// Step 3: queries
// ---------------------------------------------------------------------------

// Find(pred, args...) matches the pattern against EVERY fact in FactDB.
// For each match, every variable in the pattern appends its value to its List.
// The lists stay aligned: index i of X, Y, Z together form the i-th answer tuple.
// Returns the number of matching facts (so a query with no variables, e.g.
// Find(likes, john, mary), works as a yes/no question).
template <typename... Ts>
size_t Find(const Pred& p, const Ts&... ts)
{
    Pattern pat(p, ts...);               // same variadic pack, forwarded unchanged

    // Distinct variables of the pattern, in order. likes(X, X) lists X once.
    vector<const VARIABLE*> vars;
    for (const Term& t : pat.terms) {
        if (holds_alternative<const VARIABLE*>(t)) {
            const VARIABLE* v = get<const VARIABLE*>(t);
            if (find(vars.begin(), vars.end(), v) == vars.end())
                vars.push_back(v);
        }
    }

    // A new query replaces the old answers of its variables.
    for (const VARIABLE* v : vars) v->values.clear();

    size_t count = 0;
    for (const Fact& f : FactDB::all()) {
        if (optional<Bindings> b = match(pat, f)) {
            for (const VARIABLE* v : vars) v->values.add(b->at(v));
            count++;
        }
    }
    return count;
}

// ---------------------------------------------------------------------------
// Step 4: rules and forward chaining
// ---------------------------------------------------------------------------

// A rule's argument list mixes all three kinds. Every Pred starts a new pattern.
using Arg = variant<const Pred*, const Obj*, const VARIABLE*>;

// Rule(likes, X, Y,  likes, Y, Z,  likes, X, Z)
//      \________/    \________/    \________/
//       body[0]       body[1]        head        (the LAST pattern is the head)
// Meaning: likes(X,Y) AND likes(Y,Z)  =>  likes(X,Z)
class Rule {
public:
    vector<Pattern> body;   // conditions: all must hold at the same time ...
    Pattern head;           // ... and then this (filled in) becomes a new fact

    // First parameter is a Pred, so a rule must start with a predicate.
    template <typename... Ts>
    Rule(const Pred& p, const Ts&... ts);

    void print() const {
        head.print();
        cout << " :- ";                  // Prolog notation: "head if body"
        for (size_t i = 0; i < body.size(); i++) {
            if (i > 0) cout << ", ";
            body[i].print();
        }
        cout << "\n";
    }

private:
    // Delegating constructor target: last pattern is the head, the rest is the body.
    Rule(const vector<Pattern>& ps)
        : body(ps.begin(), ps.end() - 1), head(ps.back()) {}

    // Pack -> flat runtime list, e.g. [likes, X, Y, likes, Y, Z, likes, X, Z].
    template <typename... Ts>
    static vector<Arg> flatten(const Ts&... ts) {
        static_assert(((is_same_v<Ts, Pred> || is_same_v<Ts, Obj> || is_same_v<Ts, VARIABLE>) && ...),
                      "Rule arguments must be Pred, Obj or VARIABLE");
        return vector<Arg>{Arg{&ts}...};
    }

    // Flat list -> patterns, cutting at every Pred ("distinguish predicates from objects").
    static vector<Pattern> split(const vector<Arg>& args) {
        vector<Pattern> ps;
        for (const Arg& a : args) {
            if (holds_alternative<const Pred*>(a))
                ps.emplace_back(get<const Pred*>(a), vector<Term>{});   // start a new pattern
            else if (holds_alternative<const Obj*>(a))
                ps.back().terms.push_back(get<const Obj*>(a));          // add to current pattern
            else
                ps.back().terms.push_back(get<const VARIABLE*>(a));
        }
        if (ps.size() < 2)
            throw invalid_argument("Rule needs at least one body pattern and a head");
        return ps;
    }

    // Safety: every head variable must appear in the body, otherwise the
    // engine could never give it a value when building the new fact.
    void checkHeadVariables() const {
        for (const Term& t : head.terms) {
            if (!holds_alternative<const VARIABLE*>(t)) continue;
            const VARIABLE* v = get<const VARIABLE*>(t);
            bool found = false;
            for (const Pattern& b : body)
                for (const Term& bt : b.terms)
                    if (holds_alternative<const VARIABLE*>(bt) && get<const VARIABLE*>(bt) == v)
                        found = true;
            if (!found)
                throw invalid_argument("Rule head variable " + v->name + " does not appear in the body");
        }
    }
};

// The global store of rules + the forward-chaining engine.
class RuleDB {
public:
    static void add(const Rule& r) { rules.push_back(r); }

    // Forward chaining: apply every rule to every fact, add the new facts,
    // and repeat until a full pass adds nothing (the fixpoint).
    // Always terminates: rules only combine existing objects, never create
    // new ones, so the number of possible facts is finite.
    static void run() {
        bool changed = true;
        while (changed) {
            changed = false;

            // 1. Collect candidate facts WITHOUT touching FactDB: adding to the
            //    vector while solve() loops over it could reallocate it and
            //    invalidate the loop.
            vector<Fact> candidates;
            for (const Rule& r : rules)
                solve(r, 0, {}, candidates);

            // 2. Add the ones that are genuinely new.
            for (const Fact& f : candidates) {
                if (!FactDB::contains(f)) {
                    FactDB::add(f);
                    changed = true;
                }
            }
        }
    }

    static void dump() {
        cout << "---- RuleDB (" << rules.size() << " rules) ----\n";
        for (const Rule& r : rules) r.print();
    }

private:
    inline static vector<Rule> rules;

    // Backtracking search: satisfy body[i], body[i+1], ... in order.
    // `b` holds the bindings made by body[0..i-1]. Each fact that matches
    // body[i] consistently with `b` leads one level deeper; when all body
    // patterns are satisfied, fill the head's variables in to get a new fact.
    static void solve(const Rule& r, size_t i, const Bindings& b, vector<Fact>& out) {
        if (i == r.body.size()) {
            vector<const Obj*> args;
            for (const Term& t : r.head.terms) {
                if (holds_alternative<const Obj*>(t)) args.push_back(get<const Obj*>(t));
                else                                  args.push_back(b.at(get<const VARIABLE*>(t)));
            }
            out.emplace_back(r.head.pred, args, true);
            return;
        }
        for (const Fact& f : FactDB::all())
            if (optional<Bindings> nb = match(r.body[i], f, b))   // b itself is untouched on failure
                solve(r, i + 1, *nb, out);
    }
};

// Defined after RuleDB so that RuleDB is visible here (same reason as Fact's constructor).
template <typename... Ts>
Rule::Rule(const Pred& p, const Ts&... ts)
    : Rule(split(flatten(p, ts...)))     // delegate: pack -> flat list -> patterns
{
    checkHeadVariables();
    RuleDB::add(*this);                  // a bare Rule(...) statement registers itself
    RuleDB::run();                       // and immediately derives everything it can
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

    // Step 4: rules (seed typos fixed: missing commas, A and jean declared above)
    // Each Rule registers itself and immediately runs forward chaining.
    Rule (likes, X, Y, likes, Y, Z, likes, X, Z);   // variadic
        //distinguish predicates from objects
    Rule (likes, X, Y, Z, likes, Z, A, likes, X, A); // variadic rule
    Rule (likes, X, mary, likes, X, jean);          // variadic rule

    cout << "\n";
    RuleDB::dump();

    // Step 3: queries
    cout << "\n---- Step 3: Find() ----\n";

    cout << "?- likes(X, Y)       " << Find (likes, X, Y) << " match(es)\n";    // create lists X,Y for corresponding elements in tuples
    X.print(); Y.print();

    cout << "?- likes(john, X)    " << Find (likes, john, X) << " match(es)\n"; // first arg is committed, creates a list in X
    X.print();

    cout << "?- likes(Y, mary)    " << Find (likes, Y, mary) << " match(es)\n"; // second arg is committed, creates a list in Y
    Y.print();

    cout << "?- loves(X, Y, Z)    " << Find (loves, X, Y, Z) << " match(es)\n"; // creates lists in X,Y,Z for matching elements forming tuples
    X.print(); Y.print(); Z.print();

    cout << "?- likes(john, mary) " << Find (likes, john, mary) << " match(es)\n"; // no variables: a yes/no question

    cout << "\n";
    FactDB::dump();  // will list all facts, including newly added

    // you can also print lists in variables X,Y,Z,A etc after each fact finding
    // statement. have your own print statement like dump();
}
