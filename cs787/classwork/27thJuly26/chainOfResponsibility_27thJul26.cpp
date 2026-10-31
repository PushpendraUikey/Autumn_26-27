#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept>

// Handler<T> : base class for Chain of Responsibility.
// Subclass will only implement process(); handle() and chaining are generic.

template <typename T>
class Handler {
    public: 
        virtual ~Handler() = default;

        // public entry point for processing a request, runs current handler's logic
        // then forwards the (possibly transformed) request to next hander in the chain.
        T handle(T request) {
            T result = process(request);
            if (next_) {
                return next_ -> handle(result);
            }
            return result;
        }
        
        void setNext(std::shared_ptr<Handler<T>> next) {
            next_ = std::move(next); // moving all the resources.
        }

    protected:
        // Each concrete handler will implement this method to process the request.
        virtual T process(T request) =  0;

    private:
        std::shared_ptr<Handler<T>> next_;
};

// Pipeline<T> : builds the chain internally from a list of handlers.
// Callers never call setNext() themselves -- this is the whole point
// of keeping chaining logic out of main().

template <typename T>
class Pipeline {
    public:
        explicit Pipeline(std::vector<std::shared_ptr<Handler<T>>> handlers) {
            if (handlers.empty()) {
                throw std::invalid_argument("Pipeline needs at least one handler");
            }
            for (size_t i = 0; i < handlers.size() - 1; i++) {
                handlers[i] -> setNext(handlers[i + 1]);
            }
            handlers_ = std::move(handlers[0]);
        }

        T run(T request) {
            return handlers_ -> handle(request);
        }

    private:
        std::shared_ptr<Handler<T>> handlers_;
};

// Concrete handlers
#define DEFINE_HANDLER(NAME, EXPR) \
    class NAME : public Handler<int> { \
        protected: \
            int process(int x) override { \
                int out = (EXPR);    \
                std::cout << #NAME << ": " << x << " -> " << out << std::endl; \
                return out;                 \
            } \
        };

DEFINE_HANDLER(T1, x+1)
DEFINE_HANDLER(T2, x*3)
DEFINE_HANDLER(T3, x-3)
DEFINE_HANDLER(T5, x+10)

DEFINE_HANDLER(T6, x/2)
DEFINE_HANDLER(T7, x%3)
DEFINE_HANDLER(T8, x<<1)
DEFINE_HANDLER(T4, x*x)

#undef DEFINE_HANDLER


int main() {
    Pipeline<int> pipelineA({
        std::make_shared<T1>(),
        std::make_shared<T2>(),
        std::make_shared<T3>(),
        std::make_shared<T5>()
    });
    Pipeline<int> pipelineB({
        std::make_shared<T6>(),
        std::make_shared<T7>(),
        std::make_shared<T8>(),
        std::make_shared<T4>()
    });

    std::cout << "Pipeline A: (t1 -> t2 -> t3 -> t5) starting with 5" << std::endl;
    int resultA = pipelineA.run(5);
    std::cout << "Pipeline A result: " << resultA << "\n\n";

    std::cout << "Pipeline B: (t6 -> t7 -> t8 -> t4) starting with 5" << std::endl;
    int resultB = pipelineB.run(5);
    std::cout << "Pipeline B result: " << resultB << "\n\n";
    return 0;

}


