                  <<interface>>
                     Visitor
                  /           \
        visit(Circle)      visit(Rectangle)
              ↑                  ↑
              │                  │
       DrawVisitor          AreaVisitor
       ExportVisitor        ...


                  <<interface>>
                     Element
                  /          \
                 /            \
             Circle        Rectangle
                │               │
             accept()        accept()
                │               │
                └───────┬───────┘
                        │
                  visitor.visit()



                       Visitor
                    /     |      \
                   /      |       \
              visit(A) visit(B) visit(C)
                 ↑        ↑        ↑
                 │        │        │
             Concrete Visitors
                 │
                 │
              Element
             /   |    \
            A    B     C
            │    │     │
         accept accept accept
            │    │     │
            └────┴─────┘
                  ↑
                  │
            ObjectStructure
         (contains A, B, C)