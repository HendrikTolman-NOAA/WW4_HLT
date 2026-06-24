# WAVEWATCH IV Architecture

## Integrated Runtime Solver Selection

WW4 uses an integrated **Strategy** and **Factory** design pattern. To support complex interleaving of physics and dynamics (e.g., implicit sub-stepping), physical source terms are managed and called directly by a **Solver**.

```mermaid
classDiagram
    class WaveModel {
        -unique_ptr~ISolver~ solver_
        -vector~double~ data_
        +initialize(unique_ptr~ISolver~ solver)
        +setData(vector~double~ initialData)
        +step()
        +getData() span~const double~
    }

    class ISolver {
        <<interface>>
        +getName() string_view
        +addSourceTerm(unique_ptr~ISourceTerm~ source)*
        +solve(span~double~ data)*
    }

    class ISourceTerm {
        <<interface>>
        +getName() string_view
        +calculate(span~double~ data)*
    }

    class SolverPr3 {
        -vector~unique_ptr~ISourceTerm~~ sourceTerms_
        +getName() string_view
        +addSourceTerm(unique_ptr~ISourceTerm~ source)
        +solve(span~double~ data)
    }

    class SourceSt4 {
        +getName() string_view
        +calculate(span~double~ data)
    }

    class SchemeFactory {
        +createSolver(string name) unique_ptr~ISolver~
        +createSourceTerm(string name) unique_ptr~ISourceTerm~
    }

    WaveModel o-- ISolver : orchestrates
    ISolver <|-- SolverPr3
    SolverPr3 o-- ISourceTerm : manages & calls
    ISourceTerm <|-- SourceSt4
    SchemeFactory ..> ISolver : creates
    SchemeFactory ..> ISourceTerm : creates
```

### Key Components

1.  **ISolver**: The primary integration strategy. It defines the contract for numerical solvers that combine dynamics (propagation) and physics (source terms) to advance the model state.
2.  **ISourceTerm**: The physical strategy interface. Implementations (e.g., `SourceSt4`) are injected into the solver.
3.  **SolverPr3**: A concrete solver that manages a collection of source terms and executes them during its `solve` phase.
4.  **SchemeFactory**: Responsible for instantiating individual model components (solvers and source terms).
5.  **WaveModel**: The high-level orchestrator. It manages the simulation loop and triggers the configured solver at each time step.
