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